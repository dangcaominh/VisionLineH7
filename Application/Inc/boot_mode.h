/**
 ******************************************************************************
 * @file    boot_mode.h
 * @brief   Shared boot-mode protocol definitions.
 ******************************************************************************
 */

#ifndef BOOT_MODE_H
#define BOOT_MODE_H

#include <stdbool.h>
#include <stdint.h>

#define BOOT_MODE_MAGIC      0x564C4D31U
#define BOOT_MODE_DFU        7U
#define BOOT_MODE_APP_MIN    0U
#define BOOT_MODE_APP_MAX    6U

void boot_mode_write(uint32_t mode);
bool boot_mode_read(uint32_t *mode);
void boot_mode_clear(void);

#endif /* BOOT_MODE_H */
