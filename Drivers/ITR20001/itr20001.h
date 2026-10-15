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

HAL_StatusTypeDef line_sensor_init(void);
HAL_StatusTypeDef line_sensor_read(uint16_t *buffer);
 
#endif /* ITR20001_H */