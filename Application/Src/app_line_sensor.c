/**
  ******************************************************************************
  * @file    app_line_sensor.c
  * @brief   Provide code for Line Sensor running mode
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
#include "app_line_sensor.h"
#include "itr20001.h"
#include "tb6612fng.h"
#include "pid.h"
#include <stdio.h>

#define LINE_SENSOR_SAMPLETIME_S  0.005F
#define LINE_SENSOR_BASE_DUTY     100.0F
#define LINE_SENSOR_STEER_LIMIT   100.0F

static uint16_t adc_buffer[SENSOR_NUM];
static line_sensor_error_t line_error;
static struct pid_controller steering_pid;


const struct sys_mode_api line_sensor_api =
{
    .init = app_line_sensor_init,
    .run = app_line_sensor_run,
    .stop = app_line_sensor_stop
};

static int app_line_sensor_init(void)
{
    tb6612fng_init();

    if (pid_init(&steering_pid, 1.8F, 1.0F, 0.2F,
                 LINE_SENSOR_SAMPLETIME_S,
                 -LINE_SENSOR_STEER_LIMIT,
                 LINE_SENSOR_STEER_LIMIT) != 0)
    {
        return -1;
    }

    return (line_sensor_init() == HAL_OK) ? 0 : -1;
}

static void app_line_sensor_run(void)
{
    static uint32_t last = 0;
    if (HAL_GetTick() - last < LINE_SENSOR_SAMPLETIME_S * 1000.0F)
    {
        return;
    }
    last = HAL_GetTick();
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 1);
    if (line_sensor_read(adc_buffer) != HAL_OK)
    {
        return;
    }
    
    if (line_sensor_calculate_error(adc_buffer, &line_error) != 0)
    {
        pid_reset(&steering_pid);
        tb6612fng_stop();
        return;
    }

    float steering;
    if (pid_update(&steering_pid, 0.0F, line_error.e_y, &steering) != 0)
    {
        return;
    }

    float left_duty = LINE_SENSOR_BASE_DUTY + steering;
    float right_duty = LINE_SENSOR_BASE_DUTY - steering;

    left_duty = pid_clamp(left_duty, 0.0F, 100.0F);
    right_duty = pid_clamp(right_duty, 0.0F, 100.0F);

    tb6612fng_standby();
    tb6612fng_set_direction(MOTOR_ALL, MOTOR_FORWARD);
    tb6612fng_set_speed(MOTOR_LEFT, left_duty);
    tb6612fng_set_speed(MOTOR_RIGHT, right_duty);
    tb6612fng_enable();
}

static void app_line_sensor_stop(void)
{
    tb6612fng_brake();
    pid_reset(&steering_pid);
}