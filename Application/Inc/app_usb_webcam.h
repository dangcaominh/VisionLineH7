/**
  ******************************************************************************
  * @file    app_usb_webcam.h
  * @brief   Provide code for USB Webcam running mode
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 Dang Cao Minh <dangcaominhheo@gmail.com>
  * SPDX-License-Identifier: Apache-2.0
  *
  ******************************************************************************
  */

#ifndef APP_USB_WEBCAM_H_
#define APP_USB_WEBCAM_H_

#define JPEG_SIZE   20 * 1024

static int usb_webcam_init();
static void usb_webcam_run();
static void usb_webcam_stop();

#endif /* APP_USB_WEBCAM_H */