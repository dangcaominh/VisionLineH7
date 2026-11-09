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

static uint8_t image_buffers[2][IMAGE_SIZE] __attribute__((section(".axi_sram"), aligned(32)));
uint8_t jpeg_buffer[JPEG_SIZE] __attribute__((section(".axi_sram"), aligned(32)));
uint8_t ycbcr_buffer[IMAGE_SIZE] __attribute__((section(".ahb_sram"), aligned(32)));

#define INPUT_CHUNK_SIZE 4096U
#define min(a, b)	(a > b) ? b : a

extern DCMI_HandleTypeDef hdcmi;
extern JPEG_HandleTypeDef hjpeg;

static volatile bool image_done = false;
static volatile uint32_t rgb_ready_mask;
static volatile uint8_t rgb_read_index;

extern DMA_HandleTypeDef hdma_dcmi;
static JPEG_RGBToYCbCr_Convert_Function convert_function = NULL;

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
}

static void camera_dma_complete(DMA_HandleTypeDef* hdma)
{
	uint8_t const completed_index = ((DMA_Stream_TypeDef*)hdma->Instance)->CR & DMA_SxCR_CT ? 0U : 1U;

	SCB_InvalidateDCache_by_Addr((uint32_t*)image_buffers[completed_index], IMAGE_SIZE);
	rgb_ready_mask |= 1U << completed_index;
	image_done = true;
}

static void camera_dma_error(DMA_HandleTypeDef* hdma)
{
	UNUSED(hdma);
	image_done = false;
}



int camera_init_rgb565(void)
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


int camera_init_yuy2(void)
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
	ov5640_yuy2_mode();
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
	if (hdcmi.State == HAL_DCMI_STATE_BUSY)
	{
		return 0;
	}

	image_done = false;
	rgb_ready_mask = 0U;
	rgb_read_index = 0U;
	SCB_InvalidateDCache_by_Addr((uint32_t*)image_buffers[0], IMAGE_SIZE);
	SCB_InvalidateDCache_by_Addr((uint32_t*)image_buffers[1], IMAGE_SIZE);

	hdma_dcmi.XferCpltCallback = camera_dma_complete;
	hdma_dcmi.XferM1CpltCallback = camera_dma_complete;
	hdma_dcmi.XferErrorCallback = camera_dma_error;
	hdcmi.State = HAL_DCMI_STATE_BUSY;

	// Enable DCMI
	__HAL_DCMI_ENABLE(&hdcmi);
	// Disable DCMI_MODE_SNAPSHOT
	hdcmi.Instance->CR &= ~DCMI_CR_CM;
	// Enable DCMI_MODE_CONTINOUS
	hdcmi.Instance->CR |= DCMI_MODE_CONTINUOUS;

	// Start DCMI double-buffer DMA
	if (HAL_DMAEx_MultiBufferStart_IT(
		&hdma_dcmi,
		(uint32_t)&hdcmi.Instance->DR,
		(uint32_t)image_buffers[0],
		(uint32_t)image_buffers[1],
		IMAGE_SIZE / 4U) != HAL_OK)
	{
		hdcmi.State = HAL_DCMI_STATE_READY;
		printf("Cannot start DCMI double-buffer DMA\r\n");
		return -1;
	}

	// Start capture
	hdcmi.Instance->CR |= DCMI_CR_CAPTURE;
	return 0;
}

uint8_t* camera_get_image()
{
	if (image_done)
	{
		for (uint8_t offset = 0U; offset < 2U; offset++)
		{
			uint8_t const index = (uint8_t)((rgb_read_index + offset) & 1U);
			if ((rgb_ready_mask & (1U << index)) != 0U)
			{
				rgb_ready_mask &= ~(1U << index);
				rgb_read_index = (uint8_t)(index ^ 1U);
				image_done = rgb_ready_mask != 0U;
				return image_buffers[index];
			}
		}
		image_done = false;
	}
	else
	{
		/* Busy */
		return NULL;
	}
	return NULL;
}

int camera_convert_jpeg(uint8_t* ptr)
{
	jpeg_data.InputOffset = 0U;
	jpeg_data.Size = 0U;
	jpeg_data.Done = false;

	if (ptr == NULL)
    {
        ptr = image_buffers[0];
	}


	SCB_CleanDCache_by_Addr((uint32_t*)ptr, IMAGE_SIZE);
	
	uint32_t converted_data_count = 0;
	convert_function(ptr, ycbcr_buffer, 0U, IMAGE_SIZE,
			 &converted_data_count);

	SCB_CleanDCache_by_Addr((uint32_t *)ycbcr_buffer, IMAGE_SIZE);
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
	HAL_DMA_Abort(&hdma_dcmi);
	HAL_DCMI_Stop(&hdcmi);
	image_done = false;
	rgb_ready_mask = 0U;
}

void HAL_JPEG_GetDataCallback(JPEG_HandleTypeDef *hjpeg, uint32_t NbEncodedData)
{
	jpeg_data.InputOffset += NbEncodedData;
	if (jpeg_data.InputOffset < IMAGE_SIZE)
	{
		HAL_JPEG_ConfigInputBuffer(
			hjpeg,
			ycbcr_buffer + jpeg_data.InputOffset,
			min(IMAGE_SIZE - jpeg_data.InputOffset, INPUT_CHUNK_SIZE));
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
