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

extern JPEG_HandleTypeDef hjpeg;

static uint32_t frame_started_ms;
static uint32_t convert_started_ms;
static uint32_t jpeg_started_ms;
static uint32_t capture_total_ms;
static uint32_t convert_total_ms;
static uint32_t jpeg_total_ms;
static uint32_t frame_total_ms;
static uint32_t profile_frames;
static uint32_t profile_start_ms;
static uint32_t interval_ms = 1000 / FRAME_RATE;

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
  static uint32_t benchmark_start_ms = 0;
  static uint32_t benchmark_start_frames = 0;
  if (profile_start_ms == 0U)
  {
    profile_start_ms = HAL_GetTick();
  }
  video_send_frame();
  uint32_t const now_ms = HAL_GetTick();
  if ((now_ms - benchmark_start_ms) >= 1000U)
  {
    uint32_t const elapsed_ms = now_ms - benchmark_start_ms;
    uint32_t const completed = video_frames_completed - benchmark_start_frames;
    printf("UVC FPS: %lu (frames: %lu, elapsed: %lu ms)\r\n",
      (unsigned long)((completed * 1000U) / elapsed_ms),
      (unsigned long)completed,
      (unsigned long)elapsed_ms);
    if (profile_frames != 0U)
    {
      printf("UVC profile interval=%lu capture=%lu convert=%lu jpeg=%lu frame=%lu ms avg over %lu\r\n",
        (unsigned long)interval_ms,
        (unsigned long)(capture_total_ms / profile_frames),
        (unsigned long)(convert_total_ms / profile_frames),
        (unsigned long)(jpeg_total_ms / profile_frames),
        (unsigned long)(frame_total_ms / profile_frames),
        (unsigned long)profile_frames);
      capture_total_ms = 0U;
      convert_total_ms = 0U;
      jpeg_total_ms = 0U;
      frame_total_ms = 0U;
      profile_frames = 0U;
      profile_start_ms = now_ms;
    }
    benchmark_start_ms = now_ms;
    benchmark_start_frames = video_frames_completed;
  }
}

static void usb_webcam_stop()
{
  
}



    static struct video_handle_t video_handle;
    static uint32_t last_frame_ms;
    uint32_t video_frames_completed;

    void video_send_frame(void) {
    if (!tud_video_n_streaming(0, 0)) {
      video_handle = (struct video_handle_t){ 0 };
      return;
    }

    /*
     * offset is used as the capture pipeline phase while a frame is being
     * prepared. Once a JPEG is ready, it is the USB transfer offset.
     */
    if (!video_handle.ready && video_handle.buffer == NULL &&
      video_handle.offset == 0U) {
      if (camera_capture() == 0) {
        frame_started_ms = HAL_GetTick();
        video_handle.offset = 1U;
      }
      return;
    }

    if (!video_handle.ready && video_handle.offset == 1U) {
      uint8_t* rgb565 = camera_get_rgb565();

      if (rgb565 != NULL) {
        uint32_t const convert_start = HAL_GetTick();
        capture_total_ms += convert_start - frame_started_ms;
        convert_started_ms = convert_start;
        if (camera_convert_jpeg(rgb565) != 0) {
          printf("Failed to convert camera frame to JPEG\r\n");
          video_handle = (struct video_handle_t){ 0 };
        }
        else {
          uint32_t const jpeg_start = HAL_GetTick();
          convert_total_ms += jpeg_start - convert_started_ms;
          jpeg_started_ms = jpeg_start;
          video_handle.offset = 2U;
        }
      }
      return;
    }

    if (!video_handle.ready && video_handle.offset == 2U) {
      uint8_t* jpeg = camera_get_jpeg_buffer();
      uint32_t jpeg_size = camera_get_jpeg_size();

      if (jpeg != NULL && jpeg_size != 0U) {
        uint32_t const frame_ready = HAL_GetTick();
        jpeg_total_ms += frame_ready - jpeg_started_ms;
        video_handle.buffer = jpeg;
        video_handle.size = jpeg_size;
        video_handle.offset = 0U;
        video_handle.ready = true;
      }
      return;
    }

    if (!video_handle.ready || video_handle.offset == video_handle.size) {
      return;
    }

    uint32_t const now = tusb_time_millis_api();
    if (now - last_frame_ms < interval_ms) {
      return;
    }

    if (tud_video_n_frame_xfer(0, 0, video_handle.buffer,
      video_handle.size)) {
      video_handle.offset = video_handle.size;
      last_frame_ms = now;
    }
  }

  void tud_video_frame_xfer_complete_cb(uint_fast8_t ctl_idx, uint_fast8_t stm_idx)
  {
    (void)ctl_idx;
    (void)stm_idx;
    video_handle = (struct video_handle_t){ 0 };
    video_frames_completed++;
    frame_total_ms += HAL_GetTick() - frame_started_ms;
    profile_frames++;
  }

  int tud_video_commit_cb(uint_fast8_t ctl_idx, uint_fast8_t stm_idx,
    video_probe_and_commit_control_t const* parameters)
  {
    (void)ctl_idx;
    (void)stm_idx;

    interval_ms = parameters->dwFrameInterval / 10000U;
    if (interval_ms == 0U) {
      interval_ms = 1U;
    }

    return VIDEO_ERROR_NONE;
  }
