/**
  ******************************************************************************
  * @file    encoder.c
  * @brief   Provide driver code for encoder of N20 motor
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 Dang Cao Minh <dangcaominhheo@gmail.com>
  * SPDX-License-Identifier: Apache-2.0
  *
  ******************************************************************************
  */

#include "main.h"
#include <stdint.h>
#include <stdio.h>
#include "encoder.h"

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

/**
 * @brief Start encoder interface decoding for one motor encoder.
 * @param encoder Encoder to initialize, either `ENCODER_LEFT` or
 *                `ENCODER_RIGHT`.
 * @return HAL status returned by `HAL_TIM_Encoder_Start`; returns
 *         `HAL_ERROR` if the encoder ID is invalid.
 */
HAL_StatusTypeDef encoder_init(encoder_t encoder)
{
    /* Board naming: connector A is logical left and uses ENC2 / TIM3. */
    if(encoder == ENCODER_LEFT)
    {
        return HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
    }
    /* Board naming: connector B is logical right and uses ENC1 / TIM4. */
    else if(encoder == ENCODER_RIGHT)
    {
        return HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
    }
    else
    {
        /* Invalid encoder id */
        printf("Invalid encoder id");
        return HAL_ERROR;
    }
}

/**
 * @brief Read the current counter value of one motor encoder.
 * @param encoder Encoder to read, either `ENCODER_LEFT` or `ENCODER_RIGHT`.
 * @return The current 16-bit timer counter value, or `HAL_ERROR` if the
 *         encoder ID is invalid.
 */
uint16_t encoder_get_count(encoder_t encoder)
{
    /* Board naming: connector A is logical left and uses ENC2 / TIM3. */
    if(encoder == ENCODER_LEFT)
    {
        return (uint16_t)__HAL_TIM_GET_COUNTER(&htim3);
    }
    /* Board naming: connector B is logical right and uses ENC1 / TIM4. */
    else if(encoder == ENCODER_RIGHT)
    {
        return (uint16_t)__HAL_TIM_GET_COUNTER(&htim4);
    }
    else
    {
        /* Invalid encoder id */
        printf("Invalid encoder id");
        return HAL_ERROR;
    }
}
 
