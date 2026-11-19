/**
  ******************************************************************************
  * @file    app_line_sensor.c
  * @brief   Provide code for Line Sensor running mode
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 Dang Cao Minh <dangcaominhheo@gmail.com>
  * SPDX-License-Identifier: Apache-2.0
  *
  ******************************************************************************
  */

#include "main.h"
#include "sys_mode.h"
#include "app_line_sensor.h"
#include "itr20001.h"
#include <stdio.h>

#ifndef LINE_SENSOR_DETECTION_THRESHOLD
#define LINE_SENSOR_DETECTION_THRESHOLD 3000U
#endif

#ifndef LINE_SENSOR_ADJACENT_SPACING_M
#define LINE_SENSOR_ADJACENT_SPACING_M 0.010F
#endif

#define LINE_SENSOR_ADC_MAX 65535U

static float line_sensor_atanf(float value)
{
    /* Polynomial approximation for atan(value), valid for |value| <= 1. */
    float value2 = value * value;
    return value *
           (1.0F +
            value2 *
            (-1.0F / 3.0F +
             value2 *
             (1.0F / 5.0F +
              value2 *
              (-1.0F / 7.0F +
               value2 * (1.0F / 9.0F)))));
}

static uint16_t adc_buffer[SENSOR_NUM];
static line_sensor_error_t line_error;

static int app_line_sensor_init(void);
static void app_line_sensor_run(void);
static void app_line_sensor_stop(void);

const struct sys_mode_api line_sensor_api =
{
    .init = app_line_sensor_init,
    .run = app_line_sensor_run,
    .stop = app_line_sensor_stop
};

static int app_line_sensor_init(void)
{

    return (line_sensor_init() == HAL_OK) ? 0 : -1;
}

static void app_line_sensor_run(void)
{
    if (line_sensor_read(adc_buffer) != HAL_OK)
    {
        return;
    }

    if (line_sensor_calculate_error(adc_buffer,
                                     LINE_SENSOR_ADJACENT_SPACING_M,
                                     &line_error) == 0)
    {
        printf("e_y: %.4f m, e_phi: %.4f rad\r\n",
               line_error.e_y,
               line_error.e_phi);
    }
    else
    {
        printf("Line not detected\r\n");
    }
}

static void app_line_sensor_stop(void)
{
    line_sensor_stop();
}

int line_sensor_calculate_error(const uint16_t adc[SENSOR_NUM],
                                 float spacing_m,
                                 line_sensor_error_t *error)
{
    if (adc == NULL || error == NULL || spacing_m <= 0.0F)
    {
        return -1;
    }

    static const float sensor_position[] =
    {
        -2.0F, -1.0F, 0.0F, 1.0F, 2.0F
    };

    float weight[SENSOR_NUM];
    float weight_sum = 0.0F;
    float weighted_position_sum = 0.0F;
    float left_weight = 0.0F;
    float right_weight = 0.0F;

    for (uint32_t i = 0; i < SENSOR_NUM; i++)
    {
        uint32_t line_signal = LINE_SENSOR_ADC_MAX - adc[i];

        if (line_signal < LINE_SENSOR_DETECTION_THRESHOLD)
        {
            line_signal = 0;
        }
        else
        {
            line_signal -= LINE_SENSOR_DETECTION_THRESHOLD;
        }

        weight[i] = (float)line_signal;
        weight_sum += weight[i];
        weighted_position_sum += sensor_position[i] * weight[i];

        if (i < 2)
        {
            left_weight += weight[i];
        }
        else if (i > 2)
        {
            right_weight += weight[i];
        }
    }

    if (weight_sum <= 1.0F)
    {
        error->e_y = 0.0F;
        error->e_phi = 0.0F;
        return -1;
    }

    error->e_y =
        (weighted_position_sum / weight_sum) * spacing_m;

    /*
     * The five sensors share the same longitudinal position, so their ADC
     * readings cannot determine the true heading exactly. This heuristic
     * estimates heading from the left/right imbalance; Kalman state
     * estimation should perform the exact heading estimate.
     */
    float normalized_imbalance =
        (right_weight - left_weight) / weight_sum;
    error->e_phi = line_sensor_atanf(normalized_imbalance);

    return 0;
}