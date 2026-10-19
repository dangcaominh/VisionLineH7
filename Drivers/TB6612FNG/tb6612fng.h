/**
  ******************************************************************************
  * @file    tb6612fng.h
  * @brief   Provide driver code for TB6612FNG motor driver
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 Dang Cao Minh <dangcaominhheo@gmail.com>
  * SPDX-License-Identifier: Apache-2.0
  *
  ******************************************************************************
  */

#include "main.h"
#include <stdint.h>

#ifndef TB6612FNG_H
#define TB6612FNG_H

typedef enum
{
    MOTOR_FORWARD,
    MOTOR_REVERSE
} direction_t;

typedef enum
{
    MOTOR_LEFT,
    MOTOR_RIGHT,
    MOTOR_ALL
} motor_id_t;

void tb6612fng_init();
int tb6612fng_set_direction(motor_id_t id, direction_t direction);
int tb6612fng_set_speed(motor_id_t id, uint32_t duty);
void tb6612fng_enable();
void tb6612fng_standby();
void tb6612fng_stop();
#endif
