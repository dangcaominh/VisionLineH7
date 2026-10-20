/**
  ******************************************************************************
  * @file    camera.c
  * @brief   Provide driver code to capture image from camera module.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 Dang Cao Minh <dangcaominhheo@gmail.com>
  * SPDX-License-Identifier: Apache-2.0
  *
  ******************************************************************************
  */

#include <stdio.h>
#include <stdbool.h>
#include "main.h"
#include "ov5640.h"
#include "camera.h"

uint8_t rgb565_buffer[RGB565_SIZE] __attribute__((section(".axi_sram"), aligned(32)));
uint8_t jpeg_buffer[JPEG_SIZE] __attribute__((section(".axi_sram"), aligned(32)));

static bool jpeg_done = false;

extern DCMI_HandleTypeDef hdcmi;
extern JPEG_HandleTypeDef hjpeg;

JPEG_ConfTypeDef jpeg_config = {
	.ColorSpace = JPEG_YCBCR_COLORSPACE,
	.ChromaSubsampling = JPEG_420_SUBSAMPLING,
	.ImageWidth = IMAGE_WIDTH,
	.ImageHeight = IMAGE_HEIGHT,
	.ImageQuality = 75,
};

int camera_init(void)
{
	if (ov5640_init())
	{
		printf("Read OV5640 ID failed\r\n");
		return -1;
	}
	ov5640_light_mode(0);
	ov5640_color_saturation(3);
	ov5640_brightness(4);
	ov5640_contrast(3);
	ov5640_sharpness(33);
	ov5640_rgb565_mode();
	ov5640_out_size_set(0, 0, 320, 240);
	
	HAL_JPEG_ConfigEncoding(&hjpeg, &jpeg_config);
	
	if (HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_SNAPSHOT, (uint32_t)rgb565_buffer, RGB565_SIZE) != HAL_OK)
	{
		printf("Failed to start DCMI DMA\r\n");
		return -1;
	}
	return 0;
}




void HAL_JPEG_EncodeCpltCallback(JPEG_HandleTypeDef *hjpeg)
{
    SCB_InvalidateDCache_by_Addr((uint32_t *)jpeg_buffer, JPEG_SIZE);
	jpeg_done = 1;
}



