/**
  ******************************************************************************
  * @file    kalman.c
  * @brief   Provide code to implement Kalman filter
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 Dang Cao Minh <dangcaominhheo@gmail.com>
  * SPDX-License-Identifier: Apache-2.0
  *
  ******************************************************************************
  */


#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include "kalman.h"

/**
 * @brief Initialize the Kalman filter.
 * 
 * @param filter Pointer to the Kalman filter structure.
 * @param n Number of state variables.
 * @param x Initial state vector of size n.
 * @param P Initial estimate error covariance matrix of size n*n.
 * @param Q Process noise covariance.
 * @param R Measurement noise covariance.
 * @return 0 on success, -1 on failure.
 */
int kalman_init(struct kalman_filter* filter, struct kalman_filter_data* data, uint16_t n, float32_t* x, float32_t* P, float32_t Q, float32_t R)
{
    if (filter == NULL || filter->data == NULL || x == NULL || P == NULL || n == 0)
    {
        printf("Invalid Kalman filter parameters\n");
        return -1;
    }

    filter->data = data;
    kalman_stop(filter);

    data->x = malloc(n * sizeof(float32_t));
    if(data->x == NULL)
    {
        printf("Failed to allocate memory for state vector x\n");
        goto x_fail;
    }
    data->P = malloc(n * n * sizeof(float32_t));
    if (data->P == NULL)
    {
        printf("Failed to allocate memory for covariance matrix P\n");
        goto P_fail;
    }
    data->S = malloc(n * n * sizeof(float32_t));
    if (data->S == NULL)
    {
        printf("Failed to allocate memory for matrix S\n");
        goto S_fail;
    }
    data->inv_S = malloc(n * n * sizeof(float32_t));
    if (data->inv_S == NULL)
    {
        printf("Failed to allocate memory for matrix inv_S\n");
        goto inv_S_fail;
    }
    data->K = malloc(n * n * sizeof(float32_t));
    if (data->K == NULL)
    {
        printf("Failed to allocate memory for Kalman gain matrix K\n");
        goto K_fail;
    }
    data->y = malloc(n * sizeof(float32_t));
    if (data->y == NULL)
    {
        printf("Failed to allocate memory for vector y\n");
        goto y_fail;
    }
    data->t = malloc(n * sizeof(float32_t));
    if (data->t == NULL)
    {
        printf("Failed to allocate memory for vector t\n");
        goto t_fail;
    }

    memcpy(data->x, x, n * sizeof(float32_t));
    memcpy(data->P, P, n * n * sizeof(float32_t));

    filter->n = n;
    arm_mat_init_f32(&filter->x, n, 1, data->x);
    arm_mat_init_f32(&filter->P, n, n, data->P);
    arm_mat_init_f32(&filter->S, n, n, data->S);
    arm_mat_init_f32(&filter->inv_S, n, n, data->inv_S);
    arm_mat_init_f32(&filter->K, n, n, data->K);
    arm_mat_init_f32(&filter->y, n, 1, data->y);
    arm_mat_init_f32(&filter->t, n, 1, data->t);
    filter->Q = Q;
    filter->R = R;
    return 0;


t_fail:
    free(data->y);
    data->y = NULL;
y_fail:
    free(data->K);
    data->K = NULL;
K_fail:
    free(data->inv_S);
    data->inv_S = NULL;
inv_S_fail:
    free(data->S);
    data->S = NULL;
S_fail:
    free(data->P);
    data->P = NULL;
P_fail:
    free(data->x);
    data->x = NULL;
x_fail:
    filter->n = 0;
    return -1;
}

int kalman_update(struct kalman_filter* filter, const float32_t* input, float32_t* output)
{
    if (filter == NULL || filter->data == NULL || input == NULL ||
        output == NULL || filter->n == 0)
    {
        return -1;
    }

    struct kalman_filter_data* data = filter->data;
    uint16_t n = filter->n;

    /* P = P + Q */
    for (int i = 0; i < n; i++)
    {
        data->P[i + n * i] += filter->Q;
    }

    /* S = S + R = P + R */
    memcpy(data->S, data->P, n * n * sizeof(float32_t));
    for (int i = 0; i < n; i++)
    {
        data->S[i + n * i] += filter->R;
    }

    /* Calculate inverse of S */
    arm_status status = arm_mat_inverse_f32(&filter->S, &filter->inv_S);
    if (status != ARM_MATH_SUCCESS) {
        printf("Cannot inverse S\r\n");
        return -1; 
    }

    /* K = P(P+R)^{-1} */
    arm_mat_mult_f32(&filter->P, &filter->inv_S, &filter->K);
    
    /* y = z - x */
    for (int i = 0; i < n; i++)
    {
        data->y[i] = input[i] - data->x[i];
    }

    /* t = Ky */
    arm_mat_mult_f32(&filter->K, &filter->y, &filter->t);

    /* x = x + t = x + Ky */
    for (int i = 0; i < n; i++)
    {
        data->x[i] += data->t[i];
    }

    /* Use S to temporarily store KP */
    arm_mat_mult_f32(&filter->K, &filter->P, &filter->S);

    /*Use K to store P - S*/
    arm_mat_sub_f32(&filter->P, &filter->S, &filter->K);

    /* P = K = P - S = P - KP = (I-K)P */
    memcpy(data->P, data->K, n * n * sizeof(float32_t));

    memcpy(output, data->x, n * sizeof(float32_t));
    return 0;
}


void kalman_stop(struct kalman_filter* filter)
{
    if (filter == NULL || filter->data == NULL)
    {
        return;
    }

    struct kalman_filter_data* data = filter->data;

    free(data->x);
    free(data->P);
    free(data->S);
    free(data->inv_S);
    free(data->K);
    free(data->y);
    free(data->t);

    data->x = NULL;
    data->P = NULL;
    data->S = NULL;
    data->inv_S = NULL;
    data->K = NULL;
    data->y = NULL;
    data->t = NULL;
    filter->n = 0;
}

