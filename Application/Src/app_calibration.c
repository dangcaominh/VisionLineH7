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

#include "main.h"
#include "sys_mode.h"
#include "app_calibration.h"

const struct sys_mode_api calibration_api =
{
    .init = calibration_init,
    .run = calibration_run,
    .stop = calibration_stop
};

static int calibration_init()
{
    return 0;
}

static void calibration_run()
{

}

static void calibration_stop()
{

}