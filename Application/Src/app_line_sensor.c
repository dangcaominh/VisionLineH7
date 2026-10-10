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

const struct sys_mode_api line_sensor_api =
{
    .init = line_sensor_init,
    .run = line_sensor_run,
    .stop = line_sensor_stop
};

static int line_sensor_init()
{
    return 0;
}

static void line_sensor_run()
{

}

static void line_sensor_stop()
{

}