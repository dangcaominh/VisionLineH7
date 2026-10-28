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

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define MOTOR_PWM_ARR          11999U
#define DUTY_STEP_PERCENT      1U
#define DUTY_HOLD_MS           100U
#define ENCODER_SAMPLE_MS      10U
#define ENCODER_SAMPLES_PER_DUTY (DUTY_HOLD_MS / ENCODER_SAMPLE_MS)
#define MAX_DUTY_PERCENT       100U

const struct sys_mode_api test_motor_api =
{
    .init = test_motor_init,
    .run = test_motor_run,
    .stop = test_motor_stop
};

static int test_motor_init()
{
    tb6612fng_init();

    if (encoder_init(ENCODER_LEFT) != HAL_OK ||
        encoder_init(ENCODER_RIGHT) != HAL_OK)
    {
        printf("Encoder initialization failed\r\n");
        return -1;
    }

    return 0;
}




static void test_motor_run()
{
    enum
    {
        TEST_IDLE,
        TEST_RUNNING,
        TEST_DONE
    };

    static uint8_t state = TEST_IDLE;
    static uint32_t duty_percent = 0U;
    static uint32_t sample = 0U;
    static uint32_t last_sample_tick = 0U;
    static uint16_t previous_left_count = 0U;
    static uint16_t previous_right_count = 0U;
    static uint32_t left_pulses = 0U;
    static uint32_t right_pulses = 0U;
    static bool dead_zone_crossed = false;

    if (state == TEST_IDLE)
    {
        tb6612fng_set_direction(MOTOR_ALL, MOTOR_FORWARD);
        tb6612fng_enable();

        previous_left_count = encoder_get_count(ENCODER_LEFT);
        previous_right_count = encoder_get_count(ENCODER_RIGHT);
        left_pulses = 0U;
        right_pulses = 0U;
        dead_zone_crossed = false;
        duty_percent = 0U;
        sample = 0U;
        last_sample_tick = HAL_GetTick();

        printf("Motor test started, PWM ARR=%u\r\n", MOTOR_PWM_ARR);
        state = TEST_RUNNING;
    }

    if (state != TEST_RUNNING)
    {
        return;
    }

    if ((uint32_t)(HAL_GetTick() - last_sample_tick) < ENCODER_SAMPLE_MS)
    {
        return;
    }

    last_sample_tick += ENCODER_SAMPLE_MS;

    uint32_t duty = (duty_percent * MOTOR_PWM_ARR) / 100U;
    tb6612fng_set_speed(MOTOR_ALL, duty);

    uint16_t current_left_count = encoder_get_count(ENCODER_LEFT);
    uint16_t current_right_count = encoder_get_count(ENCODER_RIGHT);
    uint16_t left_delta = (uint16_t)(current_left_count - previous_left_count);
    uint16_t right_delta = (uint16_t)(current_right_count - previous_right_count);

    previous_left_count = current_left_count;
    previous_right_count = current_right_count;
    left_pulses += left_delta;
    right_pulses += right_delta;

    printf("duty=%lu%%, t=%lums, left=%u, right=%u\r\n",
           (unsigned long)duty_percent,
           (unsigned long)((sample + 1U) * ENCODER_SAMPLE_MS),
           left_delta,
           right_delta);

    sample++;

    if (sample >= ENCODER_SAMPLES_PER_DUTY)
    {
        if (!dead_zone_crossed && (left_pulses > 0U || right_pulses > 0U))
        {
            dead_zone_crossed = true;
            printf("Dead zone crossed at duty=%lu%%\r\n",
                   (unsigned long)duty_percent);
        }

        printf("duty=%lu%%, left_pulses=%lu, right_pulses=%lu\r\n",
               (unsigned long)duty_percent,
               (unsigned long)left_pulses,
               (unsigned long)right_pulses);

        duty_percent += DUTY_STEP_PERCENT;
        sample = 0U;
        left_pulses = 0U;
        right_pulses = 0U;

        if (duty_percent > MAX_DUTY_PERCENT)
        {
            tb6612fng_stop();
            state = TEST_DONE;
            printf("Motor test completed\r\n");
            return;
        }
    }
}

static void test_motor_stop()
{
    tb6612fng_stop();
}
