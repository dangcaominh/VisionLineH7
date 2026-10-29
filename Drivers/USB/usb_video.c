/**
  ******************************************************************************
  * @file    usb_video.c
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
#include "tusb.h"
#include <stdint.h>
#include "usb_video.h"

static struct video_handle_t video_handle;
static uint32_t transfer_started_ms;
static uint32_t transfer_time_ms;
static uint32_t transfer_count;

void usb_set_buffer(uint8_t* ptr, uint32_t size)
{
    if (video_handle.buffer != NULL || ptr == NULL || size == 0U)
    {
        return;
    }

    video_handle.buffer = ptr;
    video_handle.size = size;
}

void usb_send_frame(void)
{
    if (!tud_video_n_streaming(0, 0))
    {
        video_handle = (struct video_handle_t){ 0 };
		return;
	}

    if (video_handle.buffer != NULL && video_handle.offset == 0U &&
        tud_video_n_frame_xfer(0, 0, video_handle.buffer, video_handle.size))
    {
        transfer_started_ms = HAL_GetTick();
        video_handle.offset = video_handle.size;
    }
}

bool usb_video_is_busy(void)
{
    return video_handle.buffer != NULL;
}

void tud_video_frame_xfer_complete_cb(uint_fast8_t ctl_idx, uint_fast8_t stm_idx)
{
	UNUSED(ctl_idx);
    UNUSED(stm_idx);
	transfer_time_ms = HAL_GetTick() - transfer_started_ms;
	transfer_count++;
	video_handle = (struct video_handle_t){ 0 };
}

uint32_t usb_video_get_transfer_count(void)
{
	return transfer_count;
}

uint32_t usb_video_get_transfer_time_ms(void)
{
	return transfer_time_ms;
}

int tud_video_commit_cb(uint_fast8_t ctl_idx, uint_fast8_t stm_idx, video_probe_and_commit_control_t const* parameters)
{
    UNUSED(ctl_idx);
    UNUSED(stm_idx);
    UNUSED(parameters);
	return VIDEO_ERROR_NONE;
}
