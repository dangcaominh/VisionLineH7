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


#include <stdbool.h>
#include <stdint.h>

struct video_handle_t
{
	uint8_t *buffer;
	uint32_t offset;
	uint32_t size;
};

void usb_set_buffer(uint8_t* ptr, uint32_t size);
void usb_send_frame(void);
bool usb_video_is_busy(void);
uint32_t usb_video_get_transfer_count(void);
uint32_t usb_video_get_transfer_time_ms(void);
