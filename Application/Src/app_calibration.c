/**
  ******************************************************************************
  * @file    app_calibration.c
  * @brief   Provide code for calibration mode
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 Dang Cao Minh <dangcaominhheo@gmail.com>
  * SPDX-License-Identifier: Apache-2.0
  *
  ******************************************************************************
  */

#include <stdio.h>
#include "main.h"
#include "sys_mode.h"
#include "app_calibration.h"

#include "itr20001.h"

static uint16_t sensor_min[SENSOR_NUM];
static uint16_t sensor_max[SENSOR_NUM];

const struct sys_mode_api calibration_api =
{
    .init = calibration_init,
    .run = calibration_run,
    .stop = calibration_stop
};

static int calibration_init()
{
    for (int i = 0; i < SENSOR_NUM; i++)
    {
        sensor_min[i] = UINT16_MAX;
        sensor_max[i] = 0;
    }

    if (line_sensor_init() != HAL_OK)
    {
        return -1;
    }

    return 0;
}

static void calibration_run()
{
    uint16_t adc[SENSOR_NUM];

    if (line_sensor_read(adc) != HAL_OK)
    {
        return;
    }

    for (int i = 0; i < SENSOR_NUM; i++)
    {
        if (adc[i] < sensor_min[i])
        {
            sensor_min[i] = adc[i];
        }

        if (adc[i] > sensor_max[i])
        {
            sensor_max[i] = adc[i];
        }
    }
    HAL_Delay(100);
}

static void calibration_stop()
{
    printf("Calibration results:\r\n");
    printf("Sensor Min: ");
    for (int i = 0; i < SENSOR_NUM; i++)
    {
        printf("%d ", sensor_min[i]);
    }
    printf("\r\nSensor Max: ");
    for (int i = 0; i < SENSOR_NUM; i++)
    {
        printf("%d ", sensor_max[i]);
    }
    printf("\r\n");
}