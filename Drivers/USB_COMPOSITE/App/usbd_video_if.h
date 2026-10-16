/**
  ******************************************************************************
  * @file    usbd_video_if_template.h
  * @author  MCD Application Team
  * @brief   Template Header file for the video Interface application layer functions
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_VIDEO_IF_H__
#define __USBD_VIDEO_IF_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbd_video.h"
#include <stdbool.h>

struct video_handle_t
{
	uint8_t *buffer;
	uint32_t offset;
	uint32_t size;
	bool ready;		/* Buffer is ready to transmit */
};

/** VIDEO_IF Interface callback. */
extern USBD_VIDEO_ItfTypeDef USBD_VIDEO_fops_FS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/**
  * @brief  Manages the DMA full transfer complete event.
  * @retval None
  */
void TransferComplete_CallBack_FS(void);

/**
  * @brief  Manages the DMA half transfer complete event.
  * @retval None
  */
void HalfTransfer_CallBack_FS(void);

/* USER CODE BEGIN EXPORTED_FUNCTIONS */

/* USER CODE END EXPORTED_FUNCTIONS */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* USBD_VIDEO_IF_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/



