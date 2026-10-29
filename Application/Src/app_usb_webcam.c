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
#include "camera.h"
#include "tusb.h"
#include "usb_descriptor.h"
#include "usb_video.h"

const struct sys_mode_api usb_webcam_api =
{
	.init = usb_webcam_init,
	.run = usb_webcam_run,
	.stop = usb_webcam_stop
};



static int usb_webcam_init()
{
	camera_init();
	return 0;
}

static void usb_webcam_run()
{
	static uint32_t last_transfer_count;

	uint32_t const transfer_count = usb_video_get_transfer_count();
	if (transfer_count != last_transfer_count)
	{
		printf("capture=%lu ms, usb=%lu ms\r\n",
			(unsigned long)camera_get_capture_time_ms(),
			(unsigned long)usb_video_get_transfer_time_ms());
		last_transfer_count = transfer_count;
	}

	if (usb_video_is_busy())
	{
		return;
	}
	camera_capture();

	uint8_t* ptr = camera_get_rgb565();
	while (ptr == NULL)
	{
		tud_task();
		ptr = camera_get_rgb565();
	}
#if USE_MJPEG == 1
	if (camera_convert_jpeg(ptr) != 0)
	{
		return;
	}

	uint8_t* jpeg_ptr = camera_get_jpeg_buffer();

	while (jpeg_ptr == NULL)
	{
		tud_task();
		jpeg_ptr = camera_get_jpeg_buffer();
	}
	HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 1);

	uint32_t size = camera_get_jpeg_size();
	usb_set_buffer(jpeg_ptr, size);
	usb_send_frame();
#else
	usb_set_buffer(ptr, RGB565_SIZE);
	usb_send_frame();
#endif
}

static void usb_webcam_stop()
{
	camera_stop();
}
