/**
  ******************************************************************************
  * @file    app_test_motor.c
  * @brief   Provide code for test motor mode
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
#include "app_test_motor.h"
#include "tb6612fng.h"

#include <stdio.h>

const struct sys_mode_api test_motor_api =
{
    .init = test_motor_init,
    .run = test_motor_run,
    .stop = test_motor_stop
};

static int test_motor_init()
{
    tb6612fng_init();
    return 0;
}



static void test_motor_run()
{
    tb6612fng_set_direction(MOTOR_ALL, MOTOR_FORWARD);
    tb6612fng_set_speed(MOTOR_ALL, 11000);
    tb6612fng_enable();
    HAL_Delay(1000);

    printf("Done round 1\r\n");

    tb6612fng_standby();
    HAL_Delay(100);

    tb6612fng_set_direction(MOTOR_ALL, MOTOR_REVERSE);
    tb6612fng_set_speed(MOTOR_ALL, 11000);
    tb6612fng_enable();
    HAL_Delay(1000);

    printf("Done round 2\r\n");

    tb6612fng_standby();
    HAL_Delay(100);

    tb6612fng_set_direction(MOTOR_LEFT, MOTOR_REVERSE);
    tb6612fng_set_direction(MOTOR_RIGHT, MOTOR_FORWARD);
    tb6612fng_set_speed(MOTOR_ALL, 11000);
    tb6612fng_enable();
    HAL_Delay(1000);

    printf("Done round 3\r\n");

    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 1);
}

static void test_motor_stop()
{
    tb6612fng_stop();
}
