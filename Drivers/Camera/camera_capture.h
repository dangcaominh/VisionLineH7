/**
  ******************************************************************************
  * @file    camera_capture.h
  * @brief   Provide driver code to capture image from camera module.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 Dang Cao Minh <dangcaominhheo@gmail.com>
  * SPDX-License-Identifier: Apache-2.0
  *
  ******************************************************************************
  */

#ifndef CAMERA_CAPTURE_H
#define CAMERA_CAPTURE_H

#define CAMERA_CAPTURE_WIDTH       320U
#define CAMERA_CAPTURE_HEIGHT      240U
#define RGB565_SIZE                (CAMERA_CAPTURE_WIDTH * CAMERA_CAPTURE_HEIGHT * 2U)
#define JPEG_SIZE                  (32 * 1024U)

int camera_capture_init(void);
int camera_capture_start(void);
#endif /* CAMERA_CAPTURE_H */