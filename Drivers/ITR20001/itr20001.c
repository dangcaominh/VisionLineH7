/**
  ******************************************************************************
  * @file    itr20001.c
  * @brief   Provide driver code to read ITR20001/T line sensor
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
#include <stdbool.h>
#include "itr20001.h"

extern ADC_HandleTypeDef hadc1;
uint16_t adc_value[SENSOR_NUM] __attribute__((aligned(32), section(".ahb_sram")));

static volatile bool adc_complete = false;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc == &hadc1)
    {
        adc_complete = true;
    }
}

/**
 * @brief Start one DMA transfer for the line-sensor ADC.
 * @return HAL status returned by `HAL_ADC_Start_DMA`.
 */
HAL_StatusTypeDef line_sensor_init(void)
{
    SCB_InvalidateDCache_by_Addr((uint32_t *)adc_value, sizeof(adc_value));
    return HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_value, SENSOR_NUM);
}

/**
 * @brief Copy the completed ADC sample set to the caller's buffer.
 * @param buffer Destination buffer for `SENSOR_NUM` samples.
 * @return `HAL_OK` after a completed transfer, or `HAL_BUSY` while the
 *         current transfer is still running.
 */
HAL_StatusTypeDef line_sensor_read(uint16_t *buffer)
{
    if (adc_complete)
    {
        SCB_InvalidateDCache_by_Addr((uint32_t *)adc_value, sizeof(adc_value));
        adc_complete = false;

        for (int i = 0; i < SENSOR_NUM; i++)
        {
            buffer[i] = adc_value[i];
        }

        return HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_value, SENSOR_NUM);
    }

    return HAL_BUSY;
}
