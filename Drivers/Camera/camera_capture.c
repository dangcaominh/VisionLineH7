/**
  ******************************************************************************
  * @file    camera_capture.c
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
#include "camera_capture.h"

uint8_t rgb565_buffer[RGB565_SIZE] __attribute__((section(".axi_sram"), aligned(32)));
uint8_t jpeg_buffer[JPEG_SIZE] __attribute__((section(".axi_sram"), aligned(32)));

static bool jpeg_done = false;

extern DCMI_HandleTypeDef hdcmi;
extern JPEG_HandleTypeDef hjpeg;

JPEG_ConfTypeDef jpeg_config = {
	.ColorSpace = JPEG_YCBCR_COLORSPACE,
	.ChromaSubsampling = JPEG_420_SUBSAMPLING,
	.ImageWidth = CAMERA_CAPTURE_WIDTH,
	.ImageHeight = CAMERA_CAPTURE_HEIGHT,
	.ImageQuality = 75,
};

int camera_capture_init(void)
{
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
	uint32_t jpeg_file_size = HAL_JPEG_GetGetDataBytesProcessed(hjpeg);
	jpeg_done = 1;
}



