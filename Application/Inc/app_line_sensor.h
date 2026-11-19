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

typedef struct
{
	float e_y;       /* Lateral line error in meters. */
	float e_phi;     /* Approximate heading error in radians. */
} line_sensor_error_t;

/**
 * @brief Calculate lateral error and an approximate heading error.
 *
 * Sensor positions are [-2d, -d, 0, d, 2d], where d is the spacing
 * between adjacent sensors. This implementation assumes that the black
 * line produces a lower ADC value than the white surface.
 *
 * @param adc ADC values from all five sensors.
 * @param spacing_m Distance between adjacent sensors in meters.
 * @param error Output line errors.
 * @return 0 if a line is detected, -1 if no sensor sees the line.
 */
int line_sensor_calculate_error(const uint16_t adc[SENSOR_NUM],
                                 float spacing_m,
                                 line_sensor_error_t *error);

#endif /* APP_LINE_SENSOR_H_ */