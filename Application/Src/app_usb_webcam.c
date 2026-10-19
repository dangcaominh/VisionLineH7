/**
  ******************************************************************************
  * @file    app_usb_webcam.c
  * @brief   Provide code for USB Webcam running mode
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 Dang Cao Minh <dangcaominhheo@gmail.com>
  * SPDX-License-Identifier: Apache-2.0
  *
  ******************************************************************************
  */

#include "main.h"
#include "sys_mode.h"
#include <stdio.h>
#include <stdbool.h>
#include "app_usb_webcam.h"
#include "ov5640.h"
#include "usbd_video_if.h"

struct video_handle_t video;



uint8_t jpeg[JPEG_SIZE] __attribute__((aligned(32), section(".image_buffer")));


const struct sys_mode_api usb_webcam_api =
{
    .init = usb_webcam_init,
    .run = usb_webcam_run,
    .stop = usb_webcam_stop
};



static int usb_webcam_init()
{
    OV5640_Init();
    video.buffer = jpeg;
    video.offset = 0;
    video.size = 0;
    video.ready = false;
    return 0;
}

static void usb_webcam_run()
{

}

static void usb_webcam_stop()
{
    video.ready = false;
}