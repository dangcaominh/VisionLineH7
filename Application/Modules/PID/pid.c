/**
  ******************************************************************************
  * @file    pid.c
  * @brief   Provide code to implement a PID controller
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 Dang Cao Minh <dangcaominhheo@gmail.com>
  * SPDX-License-Identifier: Apache-2.0
  *
  ******************************************************************************
  */

#include "pid.h"
#include <stddef.h>

float pid_clamp(float value, float minimum, float maximum)
{
	if (value < minimum)
	{
		return minimum;
	}
	if (value > maximum)
	{
		return maximum;
	}
	return value;
}

int pid_init(struct pid_controller *controller,
             float kp,
             float ki,
             float kd,
             float sample_time,
             float output_min,
             float output_max)
{
	if (controller == NULL ||
	    sample_time <= 0.0F ||
	    output_min >= output_max)
	{
		return -1;
	}

	pid_stop(controller);

	controller->kp = kp;
	controller->ki = ki;
	controller->kd = kd;
	controller->sample_time = sample_time;
	controller->output_min = output_min;
	controller->output_max = output_max;
	controller->integral_min = output_min;
	controller->integral_max = output_max;
	controller->initialized = true;

	pid_reset(controller);
	return 0;
}

int pid_update(struct pid_controller *controller,
               float setpoint,
               float measurement,
               float *output)
{
	if (controller == NULL ||
	    output == NULL ||
	    !controller->initialized)
	{
		return -1;
	}

	float error = setpoint - measurement;

	controller->data.integral += error * controller->sample_time;
	controller->data.integral = pid_clamp(
		controller->data.integral,
		controller->integral_min,
		controller->integral_max);

	float derivative =
		(error - controller->data.previous_error) /
		controller->sample_time;

	*output =
		controller->kp * error +
		controller->ki * controller->data.integral +
		controller->kd * derivative;

	*output = pid_clamp(
		*output,
		controller->output_min,
		controller->output_max);

	controller->data.previous_error = error;
	return 0;
}

void pid_reset(struct pid_controller *controller)
{
	if (controller == NULL)
	{
		return;
	}

	controller->data.integral = 0.0F;
	controller->data.previous_error = 0.0F;
}

void pid_stop(struct pid_controller *controller)
{
	if (controller == NULL)
	{
		return;
	}

	controller->kp = 0.0F;
	controller->ki = 0.0F;
	controller->kd = 0.0F;
	controller->sample_time = 0.0F;
	controller->output_min = 0.0F;
	controller->output_max = 0.0F;
	controller->integral_min = 0.0F;
	controller->integral_max = 0.0F;
	controller->initialized = false;
	pid_reset(controller);
}