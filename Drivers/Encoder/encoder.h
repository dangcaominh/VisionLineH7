/**
  ******************************************************************************
  * @file    encoder.h
  * @brief   Provide driver code for encoder of N20 motor
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 Dang Cao Minh <dangcaominhheo@gmail.com>
  * SPDX-License-Identifier: Apache-2.0
  *
  ******************************************************************************
  */

#ifndef ENCODER_H
#define ENCODER_H

#include "main.h"
#include <stdint.h>

typedef enum
{
    ENCODER_LEFT,
    ENCODER_RIGHT
} encoder_t;

HAL_StatusTypeDef encoder_init(encoder_t encoder);
uint16_t encoder_get_count(encoder_t encoder);
 
#endif /* ENCODER_H */