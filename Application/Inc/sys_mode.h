/**
  ******************************************************************************
  * @file    sys_mode.h
  * @brief   Provide code for board running mode configruation
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 Dang Cao Minh <dangcaominhheo@gmail.com>
  * SPDX-License-Identifier: Apache-2.0
  *
  ******************************************************************************
  */

#ifndef SYS_MODE_H_
#define SYS_MODE_H_

#include <stdint.h>
#include <stdbool.h>

#include "main.h"

#define ERR_INVAL -1

/**
 * @brief System Operating Modes corresponding to DIP switch states (3-bit binary)
 */
typedef enum
{
    SYS_MODE_CALIBRATION   = 0x00,
    SYS_MODE_USB_WEBCAM    = 0x01,
    SYS_MODE_TEST_ADC      = 0x02,
    SYS_MODE_TEST_MOTOR    = 0x03,
    SYS_MODE_LINE_SENSOR   = 0x04,
    SYS_MODE_LINE_CAMERA   = 0x05,
    SYS_MODE_NORMAL        = 0x06,
    
    SYS_MODE_COUNT,                /* Total number of supported modes */
} sys_mode_t;

typedef enum
{
    SYS_MODE_IDLE,
    SYS_MODE_RUNNING,
    SYS_MODE_STOPPED
} sys_run_mode_t;

/**
 * @brief System Mode Interface Operations
 */
struct sys_mode_api
{
    int (*init)(void);
    void (*run)(void);
    void (*stop)(void);
};

/**
 * @brief System Mode Control Block
 */
typedef struct
{
    const char *name;
    sys_mode_t id;
    const struct sys_mode_api *api;
} sys_mode_handle;

/* Public Functions */
sys_mode_t sys_mode_init(void);
const sys_mode_handle *sys_mode_get_descriptor(sys_mode_t mode);
int sys_mode_run(sys_mode_t mode);
int sys_mode_stop(sys_mode_t mode);

#endif /* SYS_MODE_H_ */