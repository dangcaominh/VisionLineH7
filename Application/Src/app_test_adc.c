/**
  ******************************************************************************
  * @file    app_test_adc.c
  * @brief   Provide code for test ADC with line sensor mode 
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 Dang Cao Minh <dangcaominhheo@gmail.com>
  * SPDX-License-Identifier: Apache-2.0
  *
  ******************************************************************************
  */

#include "main.h"
#include <stdio.h>
#include "app_test_adc.h"
#include "itr20001.h"

static uint16_t sensor_min[SENSOR_NUM] = {3092, 22416, 25172, 22995, 25986};
static uint16_t sensor_max[SENSOR_NUM] = { 63456, 62931, 62918, 63271, 63080 };
static float y[SENSOR_NUM];

const struct sys_mode_api test_adc_api =
{
    .init = test_adc_init,
    .run = test_adc_run,
    .stop = test_adc_stop
};

static int test_adc_init()
{
    line_sensor_init();
    return 0;
}

static uint16_t buf[5];

static void test_adc_run()
{
    HAL_StatusTypeDef status = line_sensor_read(buf);
    if (status != HAL_OK)
    { 
        printf("Error while reading ADC\r\n");
        return;
    }
    
    for(int i = 0; i < 5; i++)
    {
        printf("ADC%d: %d ", i + 1, buf[i]);
    }
    printf("\r\n");

    for (int i = 0; i < 5; i++)
    {
        y[i] = (((float)sensor_max[i] - (float)buf[i]) / (float)(sensor_max[i] - sensor_min[i]));
        if (y[i] < 0)
        {
            y[i] = 0;
        }
        else if (y[i] > 1)
        {
            y[i] = 1;
        }
    }
    
    for (int i = 0; i < 5; i++)
    {
        printf("Normalized ADC%d: %d ", i + 1, (int)(100.00 * y[i]));
    }
    printf("\r\n");

    float center = 0;
    float acc = 0;
    for (int i = 0; i < 5; i++)
    {
        center += (y[i] * (float)16 * (float)(2 - i));
        acc += y[i];
    }
    center /= acc;
    printf("Center = %d mm\r\n", (int)center);

    HAL_Delay(1000);
}

static void test_adc_stop()
{

}
