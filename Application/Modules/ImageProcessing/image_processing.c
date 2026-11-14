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

void detect_line(uint8_t *gray)
{
    for (uint32_t y = 0; y < IMAGE_HEIGHT; y++)
    {
        uint32_t row = y * IMAGE_WIDTH;
        uint8_t previous = gray[row];
        gray[row] = 255U;

        for (uint32_t x = 1; x < IMAGE_WIDTH; x++)
        {
            uint32_t index = row + x;
            uint8_t current = gray[index];

            gray[index] = (current != previous) ? 0U : 255U;
            previous = current;
        }
    }
}
