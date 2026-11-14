/**
  ******************************************************************************
  * @file    app_test_motor.c
  * @brief   Provide code for test motor mode
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
#include "app_test_motor.h"
#include "encoder.h"
#include "tb6612fng.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

extern RNG_HandleTypeDef hrng;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim7;


#define TEST_MOTOR              MOTOR_LEFT
#define TEST_ENCODER            ENCODER_LEFT
#define DUTY_HOLD_SAMPLE        100U
#define DUTY_STEP_PERCENT       10U
#define MAX_DUTY_PERCENT        100U
#define DUTY_LEVEL_COUNT        (MAX_DUTY_PERCENT / DUTY_STEP_PERCENT)

const struct sys_mode_api test_motor_api =
{
    .init = test_motor_init,
    .run = test_motor_run,
    .stop = test_motor_stop
};

volatile int sample = 0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    if (htim->Instance == TIM7)
    {
        print_sample();
        sample++;
        if (sample == DUTY_HOLD_SAMPLE)
        {
            sample = 0;
            update_pwm();
        }
    }
}
extern sys_run_mode_t run_mode;
static uint32_t* duty_percent;
static uint32_t duty_sequence[DUTY_LEVEL_COUNT];

static uint32_t random_value()
{
    uint32_t value;

    if (HAL_RNG_GenerateRandomNumber(&hrng, &value) != HAL_OK)
    {
        printf("Random failed\r\n");
        Error_Handler();
    }

    return value;
}

static void prepare_duty_sequence(void)
{
    for (uint32_t i = 0U; i < DUTY_LEVEL_COUNT; i++)
    {
        duty_sequence[i] = (i + 1U) * DUTY_STEP_PERCENT;
    }

    for (uint32_t i = DUTY_LEVEL_COUNT - 1U; i > 0U; i--)
    {
        uint32_t swap_index = random_value() % (i + 1U);
        uint32_t value = duty_sequence[i];
        duty_sequence[i] = duty_sequence[swap_index];
        duty_sequence[swap_index] = value;
    }
}

static int test_motor_init()
{
    tb6612fng_init();

    if (encoder_init(ENCODER_LEFT) != HAL_OK)
    {
        printf("Encoder left initialization failed\r\n");
        return -1;
    }
    if (encoder_init(ENCODER_RIGHT) != HAL_OK)
    {
        printf("Encoder right initialization failed\r\n");
        return -1;
    }
    prepare_duty_sequence();
    duty_percent = &duty_sequence[0];
    printf("DATA,duty_percent,delta_left, delta_right\r\n");
    return 0;
}

static void test_motor_run()
{
    if (HAL_TIM_Base_GetState(&htim7) == HAL_TIM_STATE_BUSY)
    {
        return;
    }

    tb6612fng_standby();
    tb6612fng_set_direction(MOTOR_ALL, MOTOR_FORWARD);
    tb6612fng_set_speed(MOTOR_ALL, (float)(*duty_percent));
    tb6612fng_enable();

    if (HAL_TIM_Base_Start_IT(&htim7) != HAL_OK)
    {
        Error_Handler();
    }
}


static void test_motor_stop()
{
    tb6612fng_stop();
}

void print_sample()
{
    uint32_t delta_left = abs((int)encoder_get_delta(ENCODER_LEFT));
    uint32_t delta_right = abs((int)encoder_get_delta(ENCODER_RIGHT));
    printf("DATA,%lu,%lu,%lu\r\n", (*duty_percent), delta_left, delta_right);
}

void update_pwm()
{
    if (duty_percent >= &duty_sequence[DUTY_LEVEL_COUNT - 1U])
    {
        HAL_TIM_Base_Stop_IT(&htim7);
        tb6612fng_stop();
        run_mode = SYS_MODE_STOPPED;
        return;
    }
    duty_percent++;
    tb6612fng_standby();
    tb6612fng_set_direction(MOTOR_ALL, MOTOR_FORWARD);
    tb6612fng_set_speed(MOTOR_ALL, (float)(*duty_percent));
    tb6612fng_enable();
}
