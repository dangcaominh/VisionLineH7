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

#define PIXELS (IMAGE_WIDTH * IMAGE_HEIGHT)
extern uint8_t gray[PIXELS];
extern uint8_t ycbcr_buffer[IMAGE_SIZE];
extern DMA_HandleTypeDef hdma_memtomem_dma1_stream1;

void YUY2_To_Grayscale(const uint8_t* src, uint8_t* dst);
void Grayscale_To_YUY2(const uint8_t *gray, uint8_t *out);
void Grayscale_To_Binary(uint8_t *gray, uint8_t threshold);
void Detect_Line(uint8_t *gray);

const struct sys_mode_api line_camera_api =
{
    .init = line_camera_init,
    .run = line_camera_run,
    .stop = line_camera_stop
};

static int line_camera_init()
{
    return camera_init_yuy2();
}

static void line_camera_run()
{
    static bool capture_started;

	if (usb_video_is_busy())
	{
		return;
	}
	if (!capture_started)
	{
		if (camera_capture() == 0)
		{
			capture_started = true;
		}
		return;
    }

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
    grayscale_to_binary(gray, 128);
    detect_line(gray);
    grayscale_to_yuy2(gray, ycbcr_buffer);

    SCB_CleanDCache_by_Addr((uint32_t*)ycbcr_buffer, IMAGE_SIZE);
    
    usb_set_buffer(ycbcr_buffer, IMAGE_SIZE);
    usb_send_frame();
    
}

static void line_camera_stop()
{
    camera_stop();
}
