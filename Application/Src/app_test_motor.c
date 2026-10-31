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

extern TIM_HandleTypeDef htim1;

/* Physical connector A is logical left: BIN/PWM_B and ENC2/TIM3. */
#define TEST_MOTOR              MOTOR_LEFT
#define TEST_ENCODER            ENCODER_LEFT
#define DUTY_STEP_PERCENT       10U
#define DUTY_HOLD_MS            1000U
#define ENCODER_SAMPLE_MS       10U
#define MAX_DUTY_PERCENT        100U
#define DUTY_LEVEL_COUNT        (MAX_DUTY_PERCENT / DUTY_STEP_PERCENT)

const struct sys_mode_api test_motor_api =
{
    .init = test_motor_init,
    .run = test_motor_run,
    .stop = test_motor_stop
};

static bool test_started = false;
static bool test_completed = false;
static uint32_t test_start_tick = 0U;
static uint32_t previous_sample_tick = 0U;
static uint32_t duty_start_tick = 0U;
static uint32_t duty_percent = 0U;
static uint32_t duty_sequence[DUTY_LEVEL_COUNT];
static uint32_t duty_sequence_index = 0U;
static uint32_t total_pulses = 0U;
static uint16_t previous_encoder_count = 0U;

static uint32_t next_random_value(uint32_t *state)
{
    uint32_t value = *state;
    value ^= value << 13;
    value ^= value >> 17;
    value ^= value << 5;
    *state = value;
    return value;
}

static void prepare_duty_sequence(void)
{
    uint32_t seed = HAL_GetTick() ^ ((uint32_t)encoder_get_count(TEST_ENCODER) << 16);
    if (seed == 0U)
    {
        seed = 0x6D2B79F5U;
    }

    for (uint32_t i = 0U; i < DUTY_LEVEL_COUNT; i++)
    {
        duty_sequence[i] = (i + 1U) * DUTY_STEP_PERCENT;
    }

    for (uint32_t i = DUTY_LEVEL_COUNT - 1U; i > 0U; i--)
    {
        uint32_t swap_index = next_random_value(&seed) % (i + 1U);
        uint32_t value = duty_sequence[i];
        duty_sequence[i] = duty_sequence[swap_index];
        duty_sequence[swap_index] = value;
    }
}

static int test_motor_init()
{
    tb6612fng_init();

    if (encoder_init(TEST_ENCODER) != HAL_OK)
    {
        printf("Encoder initialization failed\r\n");
        return -1;
    }

    test_started = false;
    test_completed = false;
    return 0;
}

static void test_motor_run()
{
    if (test_completed)
    {
        return;
    }

    uint32_t now = HAL_GetTick();
    if (!test_started)
    {
        test_start_tick = now;
        previous_sample_tick = now;
        duty_start_tick = now;
        duty_sequence_index = 0U;
        prepare_duty_sequence();
        duty_percent = duty_sequence[duty_sequence_index];
        total_pulses = 0U;
        previous_encoder_count = encoder_get_count(TEST_ENCODER);

        tb6612fng_set_direction(TEST_MOTOR, MOTOR_FORWARD);
        tb6612fng_enable();
        uint32_t pwm_arr = __HAL_TIM_GET_AUTORELOAD(&htim1);
        uint32_t duty = (pwm_arr * duty_percent) / 100U;
        tb6612fng_set_speed(TEST_MOTOR, duty);
        test_started = true;
        printf("DATA,time_ms,duty_percent,delta,total\r\n");
        return;
    }

    if ((uint32_t)(now - previous_sample_tick) < ENCODER_SAMPLE_MS)
    {
        return;
    }

    previous_sample_tick = now;

    uint16_t current_count = encoder_get_count(TEST_ENCODER);
    uint16_t wrapped_delta = (uint16_t)(current_count - previous_encoder_count);
    int32_t signed_delta = (wrapped_delta <= INT16_MAX)
        ? (int32_t)wrapped_delta
        : (int32_t)wrapped_delta - 65536;
    uint32_t delta = (uint32_t)((signed_delta < 0) ? -signed_delta : signed_delta);
    previous_encoder_count = current_count;
    total_pulses += delta;

    printf("DATA,%lu,%lu,%lu,%lu\r\n",
           (unsigned long)(now - test_start_tick),
           (unsigned long)duty_percent,
           (unsigned long)delta,
           (unsigned long)total_pulses);

    if ((uint32_t)(now - duty_start_tick) >= DUTY_HOLD_MS)
    {
        if (duty_sequence_index + 1U >= DUTY_LEVEL_COUNT)
        {
            tb6612fng_stop();
            test_completed = true;
            return;
        }

        duty_sequence_index++;
        duty_percent = duty_sequence[duty_sequence_index];
        duty_start_tick = now;
        uint32_t pwm_arr = __HAL_TIM_GET_AUTORELOAD(&htim1);
        uint32_t duty = (pwm_arr * duty_percent) / 100U;
        tb6612fng_set_speed(TEST_MOTOR, duty);
    }
}

static void test_motor_stop()
{
    tb6612fng_stop();
    test_started = false;
    test_completed = false;
}
