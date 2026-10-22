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
#include "jpeg_utils.h"
#include "ov5640.h"
#include "camera.h"

uint8_t rgb565_buffer[RGB565_SIZE] __attribute__((section(".axi_sram"), aligned(32)));
uint8_t jpeg_buffer[JPEG_SIZE] __attribute__((section(".ahb_sram"), aligned(32)));
uint8_t ycbcr_buffer[RGB565_SIZE] __attribute__((section(".ahb_sram"), aligned(32)));

static uint32_t jpeg_size = 0;


volatile static bool rgb_done = false;
volatile static bool jpeg_done = false;

extern DCMI_HandleTypeDef hdcmi;
extern JPEG_HandleTypeDef hjpeg;

void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef* hdcmi)
{
	SCB_InvalidateDCache_by_Addr((uint32_t *)rgb565_buffer, RGB565_SIZE);
	rgb_done = true;
}

JPEG_ConfTypeDef jpeg_config = {
	.ColorSpace = JPEG_YCBCR_COLORSPACE,
	.ChromaSubsampling = JPEG_422_SUBSAMPLING,
	.ImageWidth = IMAGE_WIDTH,
	.ImageHeight = IMAGE_HEIGHT,
	.ImageQuality = 75,
};

int camera_init(void)
{
	HAL_GPIO_WritePin(CAM_RESET_GPIO_Port, CAM_RESET_Pin, GPIO_PIN_RESET);
	HAL_Delay(5);
	HAL_GPIO_WritePin(CAM_RESET_GPIO_Port, CAM_RESET_Pin, GPIO_PIN_SET);
	HAL_Delay(20);
	if (ov5640_init())
	{
		printf("OV5640 init error\r\n");
		return -1;
	}

	ov5640_light_mode(0);
	ov5640_color_saturation(3);
	ov5640_brightness(4);
	ov5640_contrast(3);
	ov5640_sharpness(33);
	ov5640_rgb565_mode();
	ov5640_out_size_set(0, 0, 320, 240);
	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 1);

	JPEG_InitColorTables();
	HAL_JPEG_ConfigEncoding(&hjpeg, &jpeg_config);
	
	return 0;
}

int camera_capture()
{
	HAL_StatusTypeDef status;
	HAL_DCMI_Stop(&hdcmi);
	rgb_done = false;
	SCB_InvalidateDCache_by_Addr((uint32_t*)rgb565_buffer, RGB565_SIZE);
	status = HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_SNAPSHOT, (uint32_t)rgb565_buffer, RGB565_SIZE / 4);
	if (status != HAL_OK)
	{
		printf("Cannot start DCMI DMA \r\n");
		return -1;
	}
	return 0;
}

uint8_t* camera_get_rgb565()
{
	if (rgb_done)
	{
		rgb_done = false;
		return rgb565_buffer;
	}
	else
	{
		/* Busy */
		return NULL;
	}
}

int camera_convert_jpeg(uint8_t* ptr)
{
	JPEG_RGBToYCbCr_Convert_Function convert_function = NULL;
	uint32_t mcu = 0;
	uint32_t converted_data_count = 0;

	jpeg_done = false;
	jpeg_size = 0;

	if (ptr == NULL)
    {
        ptr = rgb565_buffer;
	}

	if (JPEG_GetEncodeColorConvertFunc(&jpeg_config, &convert_function, &mcu) != HAL_OK)
	{
		printf("Failed to get color conversion function\r\n");
		return -1;
    }

	SCB_CleanDCache_by_Addr((uint32_t *)ptr, RGB565_SIZE);

	convert_function(ptr, ycbcr_buffer, 0U, RGB565_SIZE,
			 &converted_data_count);

	SCB_CleanDCache_by_Addr((uint32_t *)ycbcr_buffer, RGB565_SIZE);
	SCB_CleanDCache_by_Addr((uint32_t *)jpeg_buffer, JPEG_SIZE);
	if (HAL_JPEG_Encode(&hjpeg, ycbcr_buffer, converted_data_count,
				jpeg_buffer, JPEG_SIZE, HAL_MAX_DELAY) != HAL_OK)
	{
		return -1;
	}

	return 0;
}

uint8_t* camera_get_jpeg_buffer(void)
{
	if (jpeg_done)
	{
		return jpeg_buffer;
	}
	return NULL;
}

uint32_t camera_get_jpeg_size()
{
	if (jpeg_done)
	{
		return jpeg_size;
	}
	else
	{
		/* Busy */
		return 0;
	}
}


void HAL_JPEG_DataReadyCallback(JPEG_HandleTypeDef *hjpeg, uint8_t *pDataOut, uint32_t OutDataLength)
{
	jpeg_size += OutDataLength;
}


static bool check_jpeg_eoi(uint8_t *buf, uint32_t len)
{
    if (len < 2U)
    {
        return false;
    }

    for (uint32_t i = len + 2U;; i--)
    {
        if (buf[i] == 0xffU && buf[i + 1U] == 0xd9U)
		{
			jpeg_size = i + 2U;
			return true;
        }
		if (i == 0U)
		{
			break;
		}
    }
    return false;
}

void HAL_JPEG_EncodeCpltCallback(JPEG_HandleTypeDef* hjpeg)
{
	(void)hjpeg;
	SCB_CleanDCache_by_Addr((uint32_t*)jpeg_buffer, JPEG_SIZE);
	jpeg_done = check_jpeg_eoi(jpeg_buffer, jpeg_size);
	printf("jpeg_done = %d, jpeg_size = %u\r\n", jpeg_done, jpeg_size);
}

void HAL_JPEG_ErrorCallback(JPEG_HandleTypeDef *hjpeg)
{
	(void)hjpeg;
	jpeg_done = false;
}
