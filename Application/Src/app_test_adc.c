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
#include "app_test_adc.h"

const struct sys_mode_api test_adc_api =
{
    .init = test_adc_init,
    .run = test_adc_run,
    .stop = test_adc_stop
};

static int test_adc_init()
{
    return 0;
}

static void test_adc_run()
{

}

static void test_adc_stop()
{

}
