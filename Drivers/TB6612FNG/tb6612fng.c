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
#include "tb6612fng.h"

#include <stdio.h>
#include <stdint.h>

extern TIM_HandleTypeDef htim1;

/**
 * @brief Initialize the TB6612FNG control pins and start PWM outputs.
 *
 * Call this function only one time at initialization phase of code and call tb6612_enable.
 */
void tb6612fng_init()
{
    HAL_GPIO_WritePin(STBY_GPIO_Port, STBY_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(AIN_1_GPIO_Port, AIN_1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(AIN_2_GPIO_Port, AIN_2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(BIN_1_GPIO_Port, BIN_1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(BIN_2_GPIO_Port, BIN_2_Pin, GPIO_PIN_RESET);

    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);

    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 0);
}

/**
 * @brief Set the rotation direction of one motor.
 * @param id Motor to configure, either `MOTOR_LEFT` or `MOTOR_RIGHT`.
 * @param direction Rotation direction, either `MOTOR_FORWARD` or
 *                  `MOTOR_REVERSE`.
 * @return 0 on success; -1 if the motor ID or direction is invalid.
 */
int tb6612_set_direction(motor_id_t id, direction_t direction)
{
    GPIO_PinState in1;
    GPIO_PinState in2;
    if(direction == MOTOR_FORWARD)
    {
        in1 = GPIO_PIN_SET;
        in2 = GPIO_PIN_RESET;
    }
    else if (direction == MOTOR_REVERSE)
    {
        in1 = GPIO_PIN_RESET;
        in2 = GPIO_PIN_SET;
    }
    else
    {
        /* Invalid direction value */
        printf("Invalid direction value");
        return -1;
    }

    if(id == MOTOR_LEFT)
    {
        HAL_GPIO_WritePin(AIN_1_GPIO_Port, AIN_1_Pin, in1);
        HAL_GPIO_WritePin(AIN_2_GPIO_Port, AIN_2_Pin, in2);
    }
    else if(id == MOTOR_RIGHT)
    {
        HAL_GPIO_WritePin(BIN_1_GPIO_Port, BIN_1_Pin, in1);
        HAL_GPIO_WritePin(BIN_2_GPIO_Port, BIN_2_Pin, in2);
    }
    else
    {
        /* Invalid motor id */
        printf("Invalid motor id");
        return -1;
    }
    return 0;
}

/**
 * @brief Set the PWM duty compare value for one motor.
 * @param id Motor to configure, either `MOTOR_LEFT` or `MOTOR_RIGHT`.
 * @param duty PWM compare value used to control motor speed. Its valid range
 *             is defined by the timer period configured for `htim1`.
 * @return 0 on success; -1 if the motor ID is invalid.
 */
int tb6612fng_set_speed(motor_id_t id, uint32_t duty)
{
    if(id == MOTOR_LEFT)
    {
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, duty);
    }
    else if(id == MOTOR_RIGHT)
    {
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, duty);
    }
    else
    {
        /* Invalid motor id */
        printf("Invalid motor id");
        return -1;
    }
    return 0;
}

/**
 * @brief Enable the motor driver by releasing the standby input.
 *
 * Call this after tb6612fng_init or tb6612_standby.
 */
void tb6612fng_enable()
{
    HAL_GPIO_WritePin(STBY_GPIO_Port, STBY_Pin, GPIO_PIN_SET);
}

/**
 * @brief Put the motor driver into standby mode.
 *
 * Standby disables the driver output stage without changing the configured
 * direction or PWM compare values.
 */
void tb6612fng_standby()
{
    HAL_GPIO_WritePin(STBY_GPIO_Port, STBY_Pin, GPIO_PIN_RESET);
}

/**
 * @brief Stop both motors and clear their direction inputs.
 *
 * PWM compare values are set to zero and both motor channels are placed in
 * the stopped state.
 */
void tb6612fng_stop()
{
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 0);
    HAL_GPIO_WritePin(AIN_1_GPIO_Port, AIN_1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(AIN_2_GPIO_Port, AIN_2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(BIN_1_GPIO_Port, BIN_1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(BIN_2_GPIO_Port, BIN_2_Pin, GPIO_PIN_RESET);
}
