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
uint8_t jpeg_buffer[JPEG_SIZE] __attribute__((section(".axi_sram"), aligned(32)));
uint8_t ycbcr_buffer[RGB565_SIZE] __attribute__((section(".ahb_sram"), aligned(32)));

#define INPUT_CHUNK_SIZE 4096U
#define min(a, b)	(a > b) ? b : a

extern DCMI_HandleTypeDef hdcmi;
extern JPEG_HandleTypeDef hjpeg;

volatile static bool rgb_done = false;
volatile static uint32_t capture_started_ms;
volatile static uint32_t capture_time_ms;

static volatile JPEG_DataTypeDef jpeg_data = {
	.Buffer = jpeg_buffer,
	.Size = 0U,
	.InputOffset = 0U,
	.Done = false
};

JPEG_ConfTypeDef jpeg_config = {
	.ColorSpace = JPEG_YCBCR_COLORSPACE,
	.ChromaSubsampling = JPEG_422_SUBSAMPLING,
	.ImageWidth = IMAGE_WIDTH,
	.ImageHeight = IMAGE_HEIGHT,
	.ImageQuality = 75,
};

void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef* hdcmi)
{
	UNUSED(hdcmi);
	SCB_InvalidateDCache_by_Addr((uint32_t *)rgb565_buffer, RGB565_SIZE);
	capture_time_ms = HAL_GetTick() - capture_started_ms;
	rgb_done = true;
}

static JPEG_RGBToYCbCr_Convert_Function convert_function = NULL;


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
	HAL_JPEG_Init(&hjpeg);

	uint32_t mcu = 0;

	if (JPEG_GetEncodeColorConvertFunc(&jpeg_config, &convert_function, &mcu) != HAL_OK)
	{
		printf("Failed to get color conversion function\r\n");
		return -1;
    }

	return 0;
}


int camera_capture(void)
{
	HAL_StatusTypeDef status;
	rgb_done = false;
	capture_started_ms = HAL_GetTick();
	SCB_InvalidateDCache_by_Addr((uint32_t*)rgb565_buffer, RGB565_SIZE);

	status = HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_SNAPSHOT, (uint32_t)rgb565_buffer, RGB565_SIZE / 4U);

	if (status != HAL_OK)
	{
		printf("Cannot start DCMI DMA\r\n");
		return -1;
	}
	
	return 0;
}

uint32_t camera_get_capture_time_ms(void)
{
	return capture_time_ms;
}

uint8_t* camera_get_rgb565()
{
	if (rgb_done)
	{
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
	jpeg_data.InputOffset = 0U;
	jpeg_data.Size = 0U;
	jpeg_data.Done = false;

	if (ptr == NULL)
    {
        ptr = rgb565_buffer;
	}


	SCB_CleanDCache_by_Addr((uint32_t*)ptr, RGB565_SIZE);
	
	uint32_t converted_data_count = 0;
	convert_function(ptr, ycbcr_buffer, 0U, RGB565_SIZE,
			 &converted_data_count);

	SCB_CleanDCache_by_Addr((uint32_t *)ycbcr_buffer, RGB565_SIZE);
	SCB_CleanDCache_by_Addr((uint32_t*)jpeg_buffer, JPEG_SIZE);
	HAL_JPEG_ConfigEncoding(&hjpeg, &jpeg_config);

	if (HAL_JPEG_Encode_DMA(&hjpeg, ycbcr_buffer, INPUT_CHUNK_SIZE,
		jpeg_data.Buffer, JPEG_SIZE) != HAL_OK)
	{
		return -1;
	}

	return 0;
}

uint8_t* camera_get_jpeg_buffer(void)
{
	if (jpeg_data.Done)
	{
		return jpeg_data.Buffer;
	}
	return NULL;
}

uint32_t camera_get_jpeg_size()
{
	if (jpeg_data.Done)
	{
		return jpeg_data.Size;
	}
	else
	{
		/* Busy */
		return 0;
	}
}

void camera_stop()
{
	HAL_DCMI_Stop(&hdcmi);
}

void HAL_JPEG_GetDataCallback(JPEG_HandleTypeDef *hjpeg, uint32_t NbEncodedData)
{
	jpeg_data.InputOffset += NbEncodedData;
	if (jpeg_data.InputOffset < RGB565_SIZE)
	{
		HAL_JPEG_ConfigInputBuffer(
			hjpeg,
			ycbcr_buffer + jpeg_data.InputOffset,
			min(RGB565_SIZE - jpeg_data.InputOffset, INPUT_CHUNK_SIZE));
	}
	else
    {
		HAL_JPEG_Pause(hjpeg, JPEG_PAUSE_RESUME_INPUT);
    }
}

void HAL_JPEG_DataReadyCallback(JPEG_HandleTypeDef* hjpeg, uint8_t* pDataOut, uint32_t OutDataLength)
{
	UNUSED(hjpeg);
	UNUSED(pDataOut);
	jpeg_data.Size += OutDataLength;
}


void HAL_JPEG_EncodeCpltCallback(JPEG_HandleTypeDef* hjpeg)
{
	UNUSED(hjpeg);
	SCB_CleanDCache_by_Addr((uint32_t*)jpeg_data.Buffer, JPEG_SIZE);
	jpeg_data.Done = true;
}

void HAL_JPEG_ErrorCallback(JPEG_HandleTypeDef *hjpeg)
{
	UNUSED(hjpeg);
	jpeg_data.Done = false;
}
