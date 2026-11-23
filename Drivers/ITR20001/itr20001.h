/**
  ******************************************************************************
  * @file    itr20001.h
  * @brief   Provide driver code to read ITR20001/T line sensor
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 Dang Cao Minh <dangcaominhheo@gmail.com>
  * SPDX-License-Identifier: Apache-2.0
  *
  ******************************************************************************
  */

#ifndef ITR20001_H
#define ITR20001_H

#include "main.h"

#define SENSOR_NUM  5
#define SENSOR_SPACING 16

typedef struct
{
	float e_y;       /* Lateral line error in meters. */
	float e_phi;     /* Approximate heading error in radians. */
} line_sensor_error_t;

HAL_StatusTypeDef line_sensor_init(void);
HAL_StatusTypeDef line_sensor_read(uint16_t* buffer);
int line_sensor_calculate_error(uint16_t* adc, line_sensor_error_t* error);
HAL_StatusTypeDef line_sensor_stop(void);

#endif /* ITR20001_H */