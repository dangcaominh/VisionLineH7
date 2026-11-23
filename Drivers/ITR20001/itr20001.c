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
static const uint16_t sensor_min[SENSOR_NUM] = {3092U, 22416U, 25172U, 22995U, 25986U};
static const uint16_t sensor_max[SENSOR_NUM] = {63456U, 62931U, 62918U, 63271U, 63080U};

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

/**
 * @brief Calculate lateral error and an approximate heading error.
 *
 * Sensor positions are [-2d, -d, 0, d, 2d], where d is the spacing
 * between adjacent sensors. This implementation assumes that the black
 * line produces a lower ADC value than the white surface.
 *
 * @param adc ADC values from all five sensors.
 * @param error Output line errors.
 * @return 0 if a line is detected, -1 if no sensor sees the line.
 */
int line_sensor_calculate_error(uint16_t* adc, line_sensor_error_t* error)
{
    float weight[SENSOR_NUM];
    float weight_sum = 0.0F;
    float weighted_position = 0.0F;

    for (int i = 0; i < SENSOR_NUM; i++)
    {
        weight[i] = ((float)sensor_max[i] - (float)adc[i]) / ((float)sensor_max[i] - (float)sensor_min[i]);

        if (weight[i] < 0.0F)
        {
            weight[i] = 0.0F;
        }
        else if (weight[i] > 1.0F)
        {
            weight[i] = 1.0F;
        }

        weight_sum += weight[i];
        weighted_position += (float)(2 - i) * (float)SENSOR_SPACING * weight[i];
    }

    if (weight_sum < 0.20F)
    {
        error->e_y = 0;
        error->e_phi = 0;
        return -1;
    }

    error->e_y = weighted_position / weight_sum;
    error->e_phi = 0;
    return 0;
}

/**
 * @brief Stop the line-sensor ADC DMA transfer.
 * @return HAL status returned by `HAL_ADC_Stop_DMA`.
 */
HAL_StatusTypeDef line_sensor_stop(void)
{
    return HAL_ADC_Stop_DMA(&hadc1);
}


