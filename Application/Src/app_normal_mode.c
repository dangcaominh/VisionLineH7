/**
  ******************************************************************************
  * @file    app_normal_mode.c
  * @brief   Provide code for normal mode
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
#include "app_normal_mode.h"

const struct sys_mode_api normal_mode_api =
{
    .init = normal_mode_init,
    .run = normal_mode_run,
    .stop = normal_mode_stop
};

static int normal_mode_init()
{
    return 0;
}

static void normal_mode_run()
{

}

static void normal_mode_stop()
{

}