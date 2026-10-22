/**
  ******************************************************************************
  * @file    camera.h
  * @brief   Provide driver code to capture image from camera module.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 Dang Cao Minh <dangcaominhheo@gmail.com>
  * SPDX-License-Identifier: Apache-2.0
  *
  ******************************************************************************
  */

#ifndef CAMERA_H
#define CAMERA_H

#define IMAGE_WIDTH       320U
#define IMAGE_HEIGHT      240U
#define RGB565_SIZE                (IMAGE_WIDTH * IMAGE_HEIGHT * 2U)
#define JPEG_SIZE                  (32 * 1024U)



int camera_init(void);
int camera_capture();
uint8_t* camera_get_rgb565(void);
int camera_convert_jpeg(uint8_t* ptr);
uint8_t* camera_get_jpeg_buffer();
uint32_t camera_get_jpeg_size();


#endif /* CAMERA_CAPTURE_H */