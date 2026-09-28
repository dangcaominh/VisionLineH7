/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define PRG_MODE0_Pin GPIO_PIN_13
#define PRG_MODE0_GPIO_Port GPIOC
#define PRG_MODE1_Pin GPIO_PIN_14
#define PRG_MODE1_GPIO_Port GPIOC
#define PRG_MODE2_Pin GPIO_PIN_15
#define PRG_MODE2_GPIO_Port GPIOC
#define PWM_A_Pin GPIO_PIN_14
#define PWM_A_GPIO_Port GPIOE
#define STBY_Pin GPIO_PIN_9
#define STBY_GPIO_Port GPIOD
#define BIN_1_Pin GPIO_PIN_10
#define BIN_1_GPIO_Port GPIOD
#define BIN_2_Pin GPIO_PIN_11
#define BIN_2_GPIO_Port GPIOD
#define ENC2_A_Pin GPIO_PIN_12
#define ENC2_A_GPIO_Port GPIOD
#define ENC2_B_Pin GPIO_PIN_13
#define ENC2_B_GPIO_Port GPIOD
#define AIN_1_Pin GPIO_PIN_14
#define AIN_1_GPIO_Port GPIOD
#define AIN_2_Pin GPIO_PIN_15
#define AIN_2_GPIO_Port GPIOD
#define ENC1_A_Pin GPIO_PIN_6
#define ENC1_A_GPIO_Port GPIOC
#define ENC1_B_Pin GPIO_PIN_7
#define ENC1_B_GPIO_Port GPIOC
#define XCLK_Pin GPIO_PIN_9
#define XCLK_GPIO_Port GPIOC
#define PWM_B_Pin GPIO_PIN_8
#define PWM_B_GPIO_Port GPIOA
#define LED_Y_Pin GPIO_PIN_5
#define LED_Y_GPIO_Port GPIOD
#define LED_R_Pin GPIO_PIN_6
#define LED_R_GPIO_Port GPIOD
#define LED_G_Pin GPIO_PIN_7
#define LED_G_GPIO_Port GPIOD
#define CAM_RST_Pin GPIO_PIN_6
#define CAM_RST_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
