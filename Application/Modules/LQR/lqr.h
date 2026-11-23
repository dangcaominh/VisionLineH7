/**
  ******************************************************************************
  * @file    lqr.h
  * @brief   Provide code to implement a LQR controller
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 Dang Cao Minh <dangcaominhheo@gmail.com>
  * SPDX-License-Identifier: Apache-2.0
  *
  ******************************************************************************
  */

#ifndef LQR_H
#define LQR_H

#include <stdint.h>
#include "arm_math.h"

struct lqr_data
{
    float32_t A[4];
    float32_t A_T[4];
    float32_t B[2];
    float32_t B_T[2];
    float32_t P[4];
    float32_t Q[4];
    float32_t R;
    float32_t K[2];
};

struct lqr_controller
{
    float32_t sensor_distance;
    float32_t sample_time;
    float32_t velocity;
    float32_t output_min;
    float32_t output_max;

    arm_matrix_instance_f32 A;
    arm_matrix_instance_f32 A_T;
    arm_matrix_instance_f32 B;
    arm_matrix_instance_f32 B_T;
    arm_matrix_instance_f32 P;
    arm_matrix_instance_f32 Q;
    arm_matrix_instance_f32 K;

    struct lqr_data* data;
};

int lqr_init(struct lqr_controller* controller,
    float32_t sensor_distance,
    float32_t sample_time,
    float32_t velocity,
    float32_t q_y,
    float32_t q_phi,
    float32_t r,
    float32_t output_min,
    float32_t output_max);

int lqr_set_velocity(struct lqr_controller* controller,
    float32_t velocity);

int lqr_update(struct lqr_controller* controller,
    const float32_t state[2],
    float32_t* omega);

void lqr_reset(struct lqr_controller* controller);
void lqr_stop(struct lqr_controller* controller);

#endif /* LQR_H */
