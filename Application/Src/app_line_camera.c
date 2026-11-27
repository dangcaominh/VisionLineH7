/**
  ******************************************************************************
  * @file    app_line_camera.c
  * @brief   Provide code for Line Camera running mode
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 Dang Cao Minh <dangcaominhheo@gmail.com>
  * SPDX-License-Identifier: Apache-2.0
  *
  ******************************************************************************
  */

#include "main.h"
#include "sys_mode.h"
#include "app_line_camera.h"
#include "camera.h"
#include "usb_video.h"
#include "image_processing.h"
#include "tb6612fng.h"
#include "pid.h"
#include <stdbool.h>
#include <stdio.h>
#include "arm_math.h"

#define PIXELS (IMAGE_WIDTH * IMAGE_HEIGHT)
#define ROI_TOP 200
#define LINE_PIXEL_THRESHOLD 50U
#define RAD_TO_DEG 57.2957795F

#define LINE_CAMERA_SAMPLETIME_S 0.01F
#define LINE_CAMERA_BASE_DUTY 100.0F
#define LINE_CAMERA_STEER_LIMIT 100.0F

extern uint8_t gray[PIXELS];
extern uint8_t ycbcr_buffer[IMAGE_SIZE];
extern DMA_HandleTypeDef hdma_memtomem_dma1_stream1;
extern sys_run_mode_t run_mode;

static struct pid_controller steering_pid;

static const float Homography[3][3] = {
    { -0.3755965F, -0.0000000F, 60.09544F },
    { 0.0011823F, 0.0251213F, -113.79642F },
    { -0.0000691F, -0.0095638F, 1.00000F }
};

static arm_matrix_instance_f32 H = {
    .numRows = 3,
    .numCols = 3,
    .pData   = &Homography[0][0]
};

/**
 * @brief Chuyen toa do pixel (u, v) tren anh 320x240 sang toa do san (x_mm, y_mm)
 * @param u      Toa do cot [0 .. 319]
 * @param v      Toa do hang [0 .. 239]
 * @param x_mm   Con tro nhan toa do ngang (mm): am ben trai, duong ben phai
 * @param y_mm   Con tro nhan toa do doc (mm): khoang cach phia truoc truc banh xe
 * @return true neu diem nam duoi duong chan troi (hop le), false neu nguoc lai
 */
bool pixel_to_ground(float u, float v, float* x, float* y)
{
    float p[3] = { u, v, 1.0F };
    float q[3];
    arm_mat_vec_mult_f32(&H, p, q);

    if (q[2] <= 1e-4f && q[2] >= -1e-4f) {
        return false;
    }

    *x = q[0] / q[2];
    *y = q[1] / q[2];
    return true;
}

#define OTSU_MIN_CONTRAST_VARIANCE 150.0F // Ngưỡng phương sai tối thiểu để tránh nhận nhầm nhiễu khi mất line

/**
 * @brief Tinh nguong nhi phan dong Otsu trong vung ROI [ROI_TOP .. IMAGE_HEIGHT)
 * @param image Con tro mang anh xam 8-bit
 * @return Gia tri nguong toi uu [0..255], hoac fallback LINE_PIXEL_THRESHOLD neu vung anh dong nhat
 */
static uint8_t calculate_otsu_threshold(const uint8_t *image)
{
    uint32_t histogram[256] = {0};
    const uint32_t start_idx = ROI_TOP * IMAGE_WIDTH;
    const uint32_t end_idx = IMAGE_HEIGHT * IMAGE_WIDTH;
    const uint32_t total_pixels = end_idx - start_idx;

    // 1. Tinh histogram va tong cuong do sang trong vung ROI
    uint64_t sum_total = 0;
    for (uint32_t i = start_idx; i < end_idx; i++)
    {
        uint8_t val = image[i];
        histogram[val]++;
        sum_total += val;
    }

    // 2. Tim nguong toi da hoa phuong sai giua hai lop (Between-Class Variance)
    uint32_t weight_bg = 0;
    uint64_t sum_bg = 0;
    float max_variance = 0.0F;
    uint8_t best_threshold = LINE_PIXEL_THRESHOLD;

    for (uint32_t t = 0; t < 256; t++)
    {
        weight_bg += histogram[t];
        if (weight_bg == 0)
        {
            continue;
        }

        uint32_t weight_fg = total_pixels - weight_bg;
        if (weight_fg == 0)
        {
            break;
        }

        sum_bg += (uint64_t)t * histogram[t];

        float mean_bg = (float)sum_bg / (float)weight_bg;
        float mean_fg = (float)(sum_total - sum_bg) / (float)weight_fg;
        float diff = mean_bg - mean_fg;

        // Phuong sai giua 2 lop: wB * wF * (mB - mF)^2
        float variance = (float)weight_bg * (float)weight_fg * diff * diff;

        if (variance > max_variance)
        {
            max_variance = variance;
            best_threshold = (uint8_t)t;
        }
    }

    // Neu do tuong phan qua thap (vi du mat line hoan toan, san trang tinh), giu nguong an toan
    float normalized_variance = max_variance / ((float)total_pixels * (float)total_pixels);
    if (normalized_variance < OTSU_MIN_CONTRAST_VARIANCE)
    {
        return LINE_PIXEL_THRESHOLD;
    }

    return best_threshold;
}

static int calculate_line_error(const uint8_t *image, float *e_y, float *e_phi, float* tan_e_phi)
{
    const uint32_t upper_top = ROI_TOP;
    const uint32_t upper_bottom = ROI_TOP + ((IMAGE_HEIGHT - ROI_TOP) / 2U);
    const uint32_t lower_bottom = IMAGE_HEIGHT;

    uint32_t upper_sum_u = 0U;
    uint32_t upper_sum_v = 0U;
    uint32_t upper_count = 0U;

    uint32_t lower_sum_u = 0U;
    uint32_t lower_sum_v = 0U;
    uint32_t lower_count = 0U;

    uint8_t threshold = calculate_otsu_threshold(image);

    for (uint32_t v = upper_top; v < lower_bottom; v++)
    {
        for (uint32_t u = 0U; u < IMAGE_WIDTH; u++)
        {
            if (image[v * IMAGE_WIDTH + u] < threshold)
            {
                if (v < upper_bottom)
                {
                    upper_sum_u += u;
                    upper_sum_v += v;
                    upper_count++;
                }
                else
                {
                    lower_sum_u += u;
                    lower_sum_v += v;
                    lower_count++;
                }
            }
        }
    }

    if (upper_count == 0U || lower_count == 0U)
    {
        return 1;
    }

    float upper_u = ((float)upper_sum_u / (float)upper_count);
    float upper_v = ((float)upper_sum_v / (float)upper_count);

    float lower_u = ((float)lower_sum_u / (float)lower_count);
    float lower_v = ((float)lower_sum_v / (float)lower_count);

    float upper_x = 0U;
    float upper_y = 0U;

    float lower_x = 0U;
    float lower_y = 0U;

    pixel_to_ground(upper_u, upper_v, &upper_x, &upper_y);
    pixel_to_ground(lower_u, lower_v, &lower_x, &lower_y);

    
    float dx = upper_x - lower_x;
    float dy = upper_y - lower_y;
    *e_y = -lower_x;
    arm_atan2_f32(dx, dy, e_phi);
    *tan_e_phi = dx / dy;

    return 0;
}

const struct sys_mode_api line_camera_api =
{
    .init = line_camera_init,
    .run = line_camera_run,
    .stop = line_camera_stop
};


static int line_camera_init()
{
    tb6612fng_init();


    if (pid_init(&steering_pid,
                 2.0F, 1.2F, 0.1F,
                 LINE_CAMERA_SAMPLETIME_S,
                 -LINE_CAMERA_STEER_LIMIT,
                 LINE_CAMERA_STEER_LIMIT) != 0)
    {
        return -1;
    }

    return camera_init_yuy2();
}

static void line_camera_run()
{
    static uint32_t last = 0U;

    if (HAL_GetTick() - last < LINE_CAMERA_SAMPLETIME_S * 1000.0F)
    {
        return;
    }
    last = HAL_GetTick();

	if (usb_video_is_busy())
	{
		return;
	}
    camera_capture();
	uint8_t* ptr = camera_get_image();
    if (ptr == NULL)
    {
        return;
    }
    SCB_InvalidateDCache_by_Addr((uint32_t*)ptr, IMAGE_SIZE);
    SCB_CleanInvalidateDCache_by_Addr((uint32_t*)ycbcr_buffer, IMAGE_SIZE);

    if (HAL_DMA_Start(&hdma_memtomem_dma1_stream1, (uint32_t)ptr, (uint32_t)ycbcr_buffer, IMAGE_SIZE / sizeof(uint32_t)) != HAL_OK)
    {
        return;
    }

    if (HAL_DMA_PollForTransfer(&hdma_memtomem_dma1_stream1, HAL_DMA_FULL_TRANSFER, 100U) != HAL_OK)
    {
        HAL_DMA_Abort(&hdma_memtomem_dma1_stream1);
        return;
    }

    SCB_InvalidateDCache_by_Addr((uint32_t*)ycbcr_buffer, IMAGE_SIZE);
    yuy2_to_grayscale(ycbcr_buffer, gray);

    float e_y;
    float e_phi;
    float tan_e_phi;
    if (calculate_line_error(gray, &e_y, &e_phi, &tan_e_phi))
    {
        printf("Line not found in lower ROI\r\n");
        pid_reset(&steering_pid);
        tb6612fng_brake();
        run_mode = SYS_MODE_STOPPED;
        return;
    }

    printf("e_y=%.2f mm, e_phi=%.2f deg\r\n", e_y, e_phi * RAD_TO_DEG);
    static float old_steering = 0;
    static float old_steering2 = 0;
    float steering;
    float control_error = e_y; -2.0F * arm_sin_f32(e_phi);
    printf("Real control error=%.2f\r\n", control_error);

    if (pid_update(&steering_pid, 0.0F, control_error, &steering))
    {
        printf("Cannot update PID\r\n");
        return;
    }
    
    float left_duty = LINE_CAMERA_BASE_DUTY + old_steering2;
    float right_duty = LINE_CAMERA_BASE_DUTY - old_steering2;
    old_steering2 = old_steering;
    old_steering = steering;

    left_duty = pid_clamp(left_duty, 0.0F, 100.0F);
    right_duty = pid_clamp(right_duty, 0.0F, 100.0F);

    printf("Left duty=%.2f, Right duty=%.2f\r\n", left_duty, right_duty);
    tb6612fng_standby();
    tb6612fng_set_direction(MOTOR_ALL, MOTOR_FORWARD);
    tb6612fng_set_speed(MOTOR_LEFT, left_duty);
    tb6612fng_set_speed(MOTOR_RIGHT, right_duty);
    tb6612fng_enable();

    grayscale_to_yuy2(gray, ycbcr_buffer);

    SCB_CleanDCache_by_Addr((uint32_t*)ycbcr_buffer, IMAGE_SIZE);
    
    usb_set_buffer(ycbcr_buffer, IMAGE_SIZE);
    usb_send_frame();
}

static void line_camera_stop()
{
    camera_stop();
    tb6612fng_stop();
    pid_reset(&steering_pid);
}
