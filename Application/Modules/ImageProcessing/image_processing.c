/**
  ******************************************************************************
  * @file    image_processing.c
  * @brief   Provide code to convert YUY2 image to suitable format for line detection
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 Dang Cao Minh <dangcaominhheo@gmail.com>
  * SPDX-License-Identifier: Apache-2.0
  *
  ******************************************************************************
  */

#include <stdio.h>
#include "main.h"
#include <stdint.h>
#include <string.h>
#include "image_processing.h"

uint8_t gray[PIXELS];


void yuy2_to_grayscale(const uint8_t *src, uint8_t *dst)
{
    for (uint32_t i = 0; i < PIXELS; i += 2)
    {
        dst[i] = src[i << 1];
        dst[i + 1] = src[(i << 1) + 2];
    }
}

void grayscale_to_yuy2(const uint8_t *src, uint8_t *dst)
{
    for (uint32_t i = 0; i < PIXELS; i += 2)
    {
        int j = i << 1;
        dst[j]     = src[i];
        dst[j + 1] = 128;
        dst[j + 2] = src[i + 1];
        dst[j + 3] = 128;
    }
}

void grayscale_to_binary(uint8_t *gray, uint8_t threshold)
{
    for (uint32_t i = 0; i < PIXELS; i++)
    {
        if (gray[i] < threshold)
        {
            gray[i] = 0;
        }
        else
        {
            gray[i] = 255;
        }
    }
}

/**
 * @brief To mau 1 pixel tai toa do (u, v) tren buffer dinh dang YUY2 (Y0 U Y1 V)
 */
static void yuy2_set_pixel(uint8_t *yuy2, int32_t u, int32_t v, uint8_t y, uint8_t cb, uint8_t cr)
{
    if (u < 0 || u >= (int32_t)IMAGE_WIDTH || v < 0 || v >= (int32_t)IMAGE_HEIGHT)
    {
        return;
    }

    uint32_t row_offset = (uint32_t)v * IMAGE_WIDTH * 2U;
    uint32_t y_idx      = row_offset + (uint32_t)u * 2U;
    uint32_t uv_idx     = row_offset + ((uint32_t)u & ~1U) * 2U;

    yuy2[y_idx]      = y;
    yuy2[uv_idx + 1] = cb;
    yuy2[uv_idx + 3] = cr;
}

/**
 * @brief Ve cham tron ban kinh radius tai (cu, cv) tren buffer YUY2
 */
static void yuy2_draw_circle(uint8_t *yuy2, int32_t cu, int32_t cv, int32_t radius, uint8_t y, uint8_t cb, uint8_t cr)
{
    for (int32_t dv = -radius; dv <= radius; dv++)
    {
        for (int32_t du = -radius; du <= radius; du++)
        {
            if ((du * du + dv * dv) <= (radius * radius))
            {
                yuy2_set_pixel(yuy2, cu + du, cv + dv, y, cb, cr);
            }
        }
    }
}

/**
 * @brief Ve doan thang noi (u0, v0) va (u1, v1) tren buffer YUY2 (Bresenham)
 */
static void yuy2_draw_line(uint8_t *yuy2, int32_t u0, int32_t v0, int32_t u1, int32_t v1, uint8_t y, uint8_t cb, uint8_t cr)
{
    int32_t du  = (u1 > u0) ? (u1 - u0) : (u0 - u1);
    int32_t dv  = (v1 > v0) ? (v1 - v0) : (v0 - v1);
    int32_t su  = (u0 < u1) ? 1 : -1;
    int32_t sv  = (v0 < v1) ? 1 : -1;
    int32_t err = du - dv;

    while (1)
    {
        yuy2_set_pixel(yuy2, u0, v0, y, cb, cr);

        if (u0 == u1 && v0 == v1)
        {
            break;
        }

        int32_t e2 = 2 * err;
        if (e2 > -dv)
        {
            err -= dv;
            u0 += su;
        }
        if (e2 < du)
        {
            err += du;
            v0 += sv;
        }
    }
}

