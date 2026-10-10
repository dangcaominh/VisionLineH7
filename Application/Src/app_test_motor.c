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

const struct sys_mode_api test_motor_api =
{
    .init = test_motor_init,
    .run = test_motor_run,
    .stop = test_motor_stop
};

static int test_motor_init()
{
    return 0;
}

static void test_motor_run()
{

}

static void test_motor_stop()
{

}
