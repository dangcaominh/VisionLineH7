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
    while(1)
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
    }
}

static void test_adc_stop()
{

}
