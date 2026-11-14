/**
  ******************************************************************************
  * @file    image_processing.h
  * @brief   Provide code to convert YUY2 image to suitable format for line detection
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 Dang Cao Minh <dangcaominhheo@gmail.com>
  * SPDX-License-Identifier: Apache-2.0
  *
  ******************************************************************************
  */

#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <stdint.h>
#include "camera.h"
#include "main.h"

#define PIXELS (IMAGE_WIDTH * IMAGE_HEIGHT)
void yuy2_to_grayscale(const uint8_t* src, uint8_t* dst);
void grayscale_to_yuy2(const uint8_t* src, uint8_t* dst);
void grayscale_to_binary(uint8_t* gray, uint8_t threshold);
void detect_line(uint8_t* gray);

#endif