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
#define ROI_TOP (IMAGE_HEIGHT / 2U)
#define CAMERA_VIEW_WIDTH_MM 90.0F
#define LINE_PIXEL_THRESHOLD 128U
#define RAD_TO_DEG 57.2957795F
#define LINE_CAMERA_SAMPLETIME_S 0.02F
#define LINE_CAMERA_BASE_DUTY 50.0F
#define LINE_CAMERA_STEER_LIMIT 50.0F

static const float H[3][3] = {
    { -2.09310F,  0.0000000F,   342.396F },
    {  0.00000F,  1.5126900F, -1120.713F },
    {  0.00000F, -0.0357360F,     1.000F }
};

extern uint8_t gray[PIXELS];
extern uint8_t ycbcr_buffer[IMAGE_SIZE];
extern DMA_HandleTypeDef hdma_memtomem_dma1_stream1;

static struct pid_controller steering_pid;

static int calculate_line_error(const uint8_t *image, float *e_y, float *e_phi, float* tan_e_phi)
{
    const uint32_t upper_top = ROI_TOP;
    const uint32_t upper_bottom = ROI_TOP + ((IMAGE_HEIGHT - ROI_TOP) / 2U);
    const uint32_t lower_bottom = IMAGE_HEIGHT;
    uint32_t upper_sum = 0U;
    uint32_t upper_count = 0U;
    uint32_t lower_sum = 0U;
    uint32_t lower_count = 0U;

    for (uint32_t y = upper_top; y < lower_bottom; y++)
    {
        for (uint32_t x = 0U; x < IMAGE_WIDTH; x++)
        {
            if (image[y * IMAGE_WIDTH + x] < LINE_PIXEL_THRESHOLD)
            {
                if (y < upper_bottom)
                {
                    upper_sum += x;
                    upper_count++;
                }
                else
                {
                    lower_sum += x;
                    lower_count++;
                }
            }
        }
    }

    if (upper_count == 0U || lower_count == 0U)
    {
        return -1;
    }

    float upper_x = (float)upper_sum / (float)upper_count;
    float lower_x = (float)lower_sum / (float)lower_count;
    float image_center = (float)(IMAGE_WIDTH - 1U) * 0.5F;
    upper_x = image_center + (upper_x - image_center) * (9.0 / 12.0);
    *e_y = (image_center - lower_x) * CAMERA_VIEW_WIDTH_MM / (float)IMAGE_WIDTH;
    float dx = upper_x - lower_x;
    float dy = (float)(upper_bottom - upper_top);
    
    *tan_e_phi = dx / dy;
    arm_atan2_f32(dx, dy, e_phi);

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
                 0.6F, 0.3F, 0.075F,
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
    __DSB();
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
    __DSB();

    yuy2_to_grayscale(ycbcr_buffer, gray);

    float e_y;
    float e_phi;
    float tan_e_phi;
    if (calculate_line_error(gray, &e_y, &e_phi, &tan_e_phi) == 0)
    {
        printf("e_y=%.2f mm, e_phi=%.2f deg\r\n", e_y, e_phi * RAD_TO_DEG);

        float steering;
        float control_error = e_y + 10.0F * tan_e_phi;
        printf("Real control error=%.2f\r\n", control_error);
        if (pid_update(&steering_pid, 0.0F, control_error, &steering) == 0)
        {
            float left_duty =
                LINE_CAMERA_BASE_DUTY + steering;
            float right_duty =
                LINE_CAMERA_BASE_DUTY - steering;
                
            left_duty = pid_clamp(left_duty, 0.0F, 100.0F);
            right_duty = pid_clamp(right_duty, 0.0F, 100.0F);
            printf("Left duty=%.2f, Right duty=%.2f\r\n", left_duty, right_duty);
                


            tb6612fng_standby();
            tb6612fng_set_direction(MOTOR_LEFT, MOTOR_FORWARD);
            tb6612fng_set_direction(MOTOR_RIGHT, MOTOR_FORWARD);
            tb6612fng_set_speed(MOTOR_LEFT, left_duty);
            tb6612fng_set_speed(MOTOR_RIGHT, right_duty);
            tb6612fng_enable();
        }
    }
    else
    {
        printf("Line not found in lower ROI\r\n");
        pid_reset(&steering_pid);
        tb6612fng_brake();
    }
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
