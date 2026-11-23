/**
  ******************************************************************************
  * @file    app_line_sensor.h
  * @brief   Provide code for Line Sensor running mode
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 Dang Cao Minh <dangcaominhheo@gmail.com>
  * SPDX-License-Identifier: Apache-2.0
  *
  ******************************************************************************
  */

#ifndef APP_LINE_SENSOR_H_
#define APP_LINE_SENSOR_H_

#include <stdint.h>
#include "itr20001.h"

static int app_line_sensor_init(void);
static void app_line_sensor_run(void);
static void app_line_sensor_stop(void);

#endif /* APP_LINE_SENSOR_H_ */