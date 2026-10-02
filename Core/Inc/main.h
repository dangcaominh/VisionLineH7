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
#include "stm32h7xx_hal.h"

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
#define ADC1_Pin GPIO_PIN_1
#define ADC1_GPIO_Port GPIOC
#define ADC2_Pin GPIO_PIN_0
#define ADC2_GPIO_Port GPIOA
#define ADC3_Pin GPIO_PIN_1
#define ADC3_GPIO_Port GPIOA
#define ADC4_Pin GPIO_PIN_2
#define ADC4_GPIO_Port GPIOA
#define ADC5_Pin GPIO_PIN_5
#define ADC5_GPIO_Port GPIOC
#define LED_1_Pin GPIO_PIN_11
#define LED_1_GPIO_Port GPIOE
#define LED_2_Pin GPIO_PIN_12
#define LED_2_GPIO_Port GPIOE
#define LED_3_Pin GPIO_PIN_13
#define LED_3_GPIO_Port GPIOE
#define PWM_A_Pin GPIO_PIN_14
#define PWM_A_GPIO_Port GPIOE
#define USER_BUTTON_Pin GPIO_PIN_15
#define USER_BUTTON_GPIO_Port GPIOB
#define BIN_2_Pin GPIO_PIN_9
#define BIN_2_GPIO_Port GPIOD
#define BIN_1_Pin GPIO_PIN_10
#define BIN_1_GPIO_Port GPIOD
#define STBY_Pin GPIO_PIN_11
#define STBY_GPIO_Port GPIOD
#define ENC1_B_Pin GPIO_PIN_12
#define ENC1_B_GPIO_Port GPIOD
#define ENC1_A_Pin GPIO_PIN_13
#define ENC1_A_GPIO_Port GPIOD
#define AIN_2_Pin GPIO_PIN_14
#define AIN_2_GPIO_Port GPIOD
#define AIN_1_Pin GPIO_PIN_15
#define AIN_1_GPIO_Port GPIOD
#define ENC2_B_Pin GPIO_PIN_6
#define ENC2_B_GPIO_Port GPIOC
#define ENC2_A_Pin GPIO_PIN_7
#define ENC2_A_GPIO_Port GPIOC
#define XCLK_Pin GPIO_PIN_9
#define XCLK_GPIO_Port GPIOC
#define PWM_B_Pin GPIO_PIN_8
#define PWM_B_GPIO_Port GPIOA
#define CAM_RESET_Pin GPIO_PIN_3
#define CAM_RESET_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
