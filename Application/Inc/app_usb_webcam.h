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

#include <stdbool.h>
#include <stdint.h>


static int usb_webcam_init(void);
static void usb_webcam_run(void);
static void usb_webcam_stop(void);


#endif /* APP_USB_WEBCAM_H */