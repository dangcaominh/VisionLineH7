/**
  ******************************************************************************
  * @file    sys_mode.c
  * @brief   Provide code for board running mode configruation
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 Dang Cao Minh <dangcaominhheo@gmail.com>
  * SPDX-License-Identifier: Apache-2.0
  *
  ******************************************************************************
  */


#include "sys_mode.h"
#include "main.h"
#include <stdio.h>
#include <stddef.h>

extern struct sys_mode_api calibration_api;
extern struct sys_mode_api usb_webcam_api;
extern struct sys_mode_api test_adc_api;
extern struct sys_mode_api test_motor_api;
extern struct sys_mode_api line_sensor_api;
extern struct sys_mode_api line_camera_api;
extern struct sys_mode_api normal_mode_api;

/* Mode Lookup Table indexed by Enum Value */
static const sys_mode_handle sys_mode_table[SYS_MODE_COUNT] = {
    [SYS_MODE_CALIBRATION] = { .name = "Sensor Calibration", .id = SYS_MODE_CALIBRATION, .api = &calibration_api },
    [SYS_MODE_USB_WEBCAM]  = { .name = "USB UVC Webcam",     .id = SYS_MODE_USB_WEBCAM,  .api = &usb_webcam_api },
    [SYS_MODE_TEST_ADC]    = { .name = "ADC Diagnostics",    .id = SYS_MODE_TEST_ADC,    .api = &test_adc_api },
    [SYS_MODE_TEST_MOTOR]  = { .name = "Motor & Encoder",   .id = SYS_MODE_TEST_MOTOR,  .api = &test_motor_api },
    [SYS_MODE_LINE_SENSOR] = { .name = "Line Trace (Sensor)", .id = SYS_MODE_LINE_SENSOR, .api = &line_sensor_api },
    [SYS_MODE_LINE_CAMERA] = { .name = "Line Trace (Camera)",.id = SYS_MODE_LINE_CAMERA, .api = &line_camera_api },
    [SYS_MODE_NORMAL]      = { .name = "Main Application",   .id = SYS_MODE_NORMAL,      .api = &normal_mode_api},
};

/**
 * @brief  Lựa chọn chế độ hoạt động ngay khi vừa boot
 * @param  mode: Dữ liệu đọc từ công tắc gạt
 * @param 0x0: Sensor calibration
 * @param 0x1: Cổng USB Highspeed được dùng làm USB Webcam
 * @param 0x2: Chạy test ADC
 * @param 0x3: Chạy test motor và encoder
 * @param 0x4: Chạy application dò line bằng sensor
 * @param 0x5: Chạy application dò line bằng camera
 * @param 0x6: Chạy application chính
 * @param 0x7: Cổng USB Highspeed được dùng làm DFU
 * @param
 */
sys_mode_t sys_mode_init(void)
{
    uint8_t bit0 = (HAL_GPIO_ReadPin(PRG_MODE0_GPIO_Port, PRG_MODE0_Pin) == GPIO_PIN_SET) ? 1 : 0;
    uint8_t bit1 = (HAL_GPIO_ReadPin(PRG_MODE1_GPIO_Port, PRG_MODE1_Pin) == GPIO_PIN_SET) ? 1 : 0;
    uint8_t bit2 = (HAL_GPIO_ReadPin(PRG_MODE2_GPIO_Port, PRG_MODE2_Pin) == GPIO_PIN_SET) ? 1 : 0;

    uint8_t mode_val = (bit2 << 2) | (bit1 << 1) | (bit0 << 0);

    if (mode_val < SYS_MODE_COUNT) {
        const sys_mode_handle* handle = &sys_mode_table[mode_val];
        const struct sys_mode_api* api = handle->api;

        if (api && api->init) {
            int ret = api->init();
            if (ret != 0) {
                return ret; /* Init failed */
            }
        }
        else
        {
            printf("Invalid api struct or api init pointer\r\n");
            return ERR_INVAL;
        }
        return (sys_mode_t)mode_val;
    }
    printf("Invalid mode\r\n");
    return ERR_INVAL;
}

/**
 * @brief Run selected mode lifecycle
 */
int sys_mode_run(sys_mode_t mode)
{
    if (mode >= SYS_MODE_COUNT) {
        printf("Invalid mode\r\n");
        return ERR_INVAL;
    }

    const sys_mode_handle* handle = &sys_mode_table[mode];
    const struct sys_mode_api* api = handle->api;

    if (api && api->run) {
        api->run();
    }
    else
    {
        printf("Invalid api struct or api run pointer\r\n");
        return ERR_INVAL;
    }

    return 0;
}


/**
 * @brief Stop selected mode
 */
int sys_mode_stop(sys_mode_t mode)
{
    if (mode >= SYS_MODE_COUNT) {
        printf("Invalid mode\r\n");
        return ERR_INVAL;
    }

    const sys_mode_handle* handle = &sys_mode_table[mode];
    const struct sys_mode_api* api = handle->api;

    if (api && api->stop) {
        api->stop();
    }
    else
    {
        printf("Invalid api struct or api run pointer\r\n");
        return ERR_INVAL;
    }

    return 0;
}