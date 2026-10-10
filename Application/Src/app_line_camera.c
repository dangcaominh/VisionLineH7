/**
  ******************************************************************************
  * @file    app_line_camera.c
  * @brief   Provide code for Line Camera running mode
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
#include "app_line_camera.h"

const struct sys_mode_api line_camera_api =
{
    .init = line_camera_init,
    .run = line_camera_run,
    .stop = line_camera_stop
};

static int line_camera_init()
{
    return 0;
}

static void line_camera_run()
{

}

static void line_camera_stop()
{

}

