/**
  ******************************************************************************
  * @file    pid.h
  * @brief   Provide code to implement a PID controller
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 Dang Cao Minh <dangcaominhheo@gmail.com>
  * SPDX-License-Identifier: Apache-2.0
  *
  ******************************************************************************
  */

#ifndef PID_H
#define PID_H

#include <stdbool.h>

struct pid_data
{
	float integral;
	float previous_error;
};

struct pid_controller
{
	float kp;
	float ki;
	float kd;
	float sample_time;
	float output_min;
	float output_max;
	float integral_min;
	float integral_max;
	bool initialized;
	struct pid_data data;
};

float pid_clamp(float value, float minimum, float maximum);

/**
 * @brief Initialize the PID controller.
 *
 * @param controller Pointer to the PID controller.
 * @param kp Proportional gain.
 * @param ki Integral gain.
 * @param kd Derivative gain.
 * @param sample_time Sampling period in seconds.
 * @param output_min Minimum controller output.
 * @param output_max Maximum controller output.
 * @return 0 on success, -1 on invalid parameters.
 */
int pid_init(struct pid_controller *controller,
             float kp,
             float ki,
             float kd,
             float sample_time,
             float output_min,
             float output_max);

/**
 * @brief Update the PID output for a setpoint and measurement.
 *
 * @param controller Pointer to the PID controller.
 * @param setpoint Desired value.
 * @param measurement Measured value.
 * @param output Pointer to the calculated controller output.
 * @return 0 on success, -1 on invalid parameters.
 */
int pid_update(struct pid_controller *controller,
               float setpoint,
               float measurement,
               float *output);

/**
 * @brief Reset the integral and previous-error state.
 */
void pid_reset(struct pid_controller *controller);

/**
 * @brief Stop the controller and reset its configuration.
 */
void pid_stop(struct pid_controller *controller);

#endif /* PID_H */