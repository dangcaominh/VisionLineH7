/**
  ******************************************************************************
  * @file    kalman.h
  * @brief   Provide code to implement Kalman filter
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 Dang Cao Minh <dangcaominhheo@gmail.com>
  * SPDX-License-Identifier: Apache-2.0
  *
  ******************************************************************************
  */

#ifndef KALMAN_H
#define KALMAN_H

#include <stdint.h>
#include "arm_math.h"

struct kalman_filter_data
{
	float32_t* x; /* State vector */
	float32_t* P; /* Estimate error covariance */
	float32_t* S;
	float32_t* inv_S;
	float32_t* K;
	float32_t* y;
	float32_t* t;
};

struct kalman_filter
{
	uint16_t n;                /* Number of dimensions */

	arm_matrix_instance_f32 x; /* State vector*/
	arm_matrix_instance_f32 P; /* Estimate error covariance */
	arm_matrix_instance_f32 S; /* S = P + R */
	arm_matrix_instance_f32 inv_S; /* inv_S = inv(S) */
	arm_matrix_instance_f32 K;	/* Kalman gain */
	arm_matrix_instance_f32 y;	/* y = z - x */
	arm_matrix_instance_f32 t;	/* t = Ky */

	float32_t Q;               /* Process noise covariance */
	float32_t R;               /* Measurement noise covariance */
	struct kalman_filter_data* data;
};


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
int kalman_init(struct kalman_filter* filter, struct kalman_filter_data* data, uint16_t n, float32_t* x, float32_t* P, float32_t Q, float32_t R);

/**
 * @brief Update the filter with a measurement.
 *
 * @param filter Pointer to the Kalman filter structure.
 * @param input Measurement vector of size n.
 * @param output Updated state vector of size n.
 * @return 0 on success, -1 on failure.
 */
int kalman_update(struct kalman_filter* filter, const float32_t* input, float32_t* output);

/**
 * @brief Free all dynamically allocated Kalman filter buffers.
 */
void kalman_stop(struct kalman_filter* filter);

#endif