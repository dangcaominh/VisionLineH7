/**
  ******************************************************************************
  * @file    lqr.c
  * @brief   Provide code to implement a LQR controller
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 Dang Cao Minh <dangcaominhheo@gmail.com>
  * SPDX-License-Identifier: Apache-2.0
  *
  ******************************************************************************
  */
#include <string.h>
#include "lqr.h"


int lqr_init(struct lqr_controller* controller,
    float32_t sensor_distance,
    float32_t sample_time,
    float32_t velocity,
    float32_t q_y,
    float32_t q_phi,
    float32_t r,
    float32_t output_min,
    float32_t output_max)
{
    struct lqr_data* data = controller->data;
    controller->sensor_distance = sensor_distance;
    controller->sample_time = sample_time;
    controller->velocity = velocity;
    controller->output_min = output_min;
    controller->output_max = output_max;

    data->Q[0] = q_y;
    data->Q[1] = 0;
    data->Q[2] = 0;
    data->Q[3] = q_phi;

    arm_mat_init_f32(&controller->A, 2, 2, data->A);
    arm_mat_init_f32(&controller->A_T, 2, 2, data->A_T);
    arm_mat_init_f32(&controller->B, 2, 1, data->B);
    arm_mat_init_f32(&controller->B_T, 1, 2, data->B_T);
    arm_mat_init_f32(&controller->P, 2, 2, data->P);
    arm_mat_init_f32(&controller->Q, 2, 2, data->Q);
    arm_mat_init_f32(&controller->K, 1, 2, data->K);
    data->R = r;

    return lqr_set_velocity(controller, velocity);
}

int lqr_set_velocity(struct lqr_controller* controller, float32_t velocity)
{
    controller->velocity = velocity;
    struct lqr_data* data = controller->data;
    data->A[0] = 1;
    data->A[1] = velocity * controller->sample_time;
    data->A[2] = 0;
    data->A[3] = 1;

    data->B[0] = (-controller->sensor_distance * controller->sample_time) - (velocity * controller->sample_time * controller->sample_time / (float32_t)2);
    data->B[1] = -controller->sample_time;

    arm_mat_trans_f32(&controller->A, &controller->A_T);
    arm_mat_trans_f32(&controller->B, &controller->B_T);

    memcpy(&data->P, &data->Q, 2 * 2 * sizeof(float32_t));

    float32_t xData[2], yData[4], zData[4];
    arm_matrix_instance_f32 x, y, z;
    arm_mat_init_f32(&x, 1, 2, xData);
    arm_mat_init_f32(&y, 2, 2, yData);
    arm_mat_init_f32(&z, 2, 2, zData);

    for (int i = 1; i <= 100; i++)
    {
        arm_mat_mult_f32(&controller->B_T, &controller->P, &x);

        float32_t S = xData[0] * data->B[0] + xData[1] * data->B[1] + data->R;
        arm_mat_mult_f32(&x, &controller->A, &controller->K);
        arm_mat_scale_f32(&controller->K, (1.0f / S), &controller->K);

        arm_mat_mult_f32(&controller->A_T, &controller->P, &z);
        arm_mat_mult_f32(&controller->B, &controller->K, &y);
        arm_mat_sub_f32(&controller->A, &y, &y);

        arm_mat_mult_f32(&z, &y, &controller->P);
        arm_mat_add_f32(&controller->P, &controller->Q, &controller->P);
    }
    return 0;
}

int lqr_update(struct lqr_controller* controller, const float32_t state[2], float32_t* omega)
{
    struct lqr_data* data = controller->data;
    *omega = -(data->K[0] * state[0] + data->K[1] * state[1]);
    if(*omega < controller->output_min)
    {
        *omega = controller->output_min;
    }
    else if(*omega > controller->output_max)
    {
        *omega = controller->output_max;
    }
    return 0;
}

void lqr_reset(struct lqr_controller* controller)
{
    struct lqr_data* data = controller->data;
    memcpy(&data->P, &data->Q, 2 * 2 * sizeof(float32_t));
    data->K[0] = 0;
    data->K[1] = 0; 
}

void lqr_stop(struct lqr_controller* controller)
{
    
}