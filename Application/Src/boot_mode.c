/**
 ******************************************************************************
 * @file    boot_mode.c
 * @brief   RTC backup-register boot-mode storage.
 ******************************************************************************
 */

#include "boot_mode.h"
#include "main.h"

static void boot_mode_enable_backup_domain(void)
{
    __HAL_RCC_RTC_ENABLE();
    HAL_PWR_EnableBkUpAccess();
}

void boot_mode_write(uint32_t mode)
{
    boot_mode_enable_backup_domain();
    RTC->BKP0R = BOOT_MODE_MAGIC;
    RTC->BKP1R = mode;
}

bool boot_mode_read(uint32_t *mode)
{
    if (mode == NULL)
    {
        return false;
    }

    boot_mode_enable_backup_domain();

    if (RTC->BKP0R != BOOT_MODE_MAGIC)
    {
        return false;
    }

    *mode = RTC->BKP1R;
    return (*mode <= BOOT_MODE_DFU);
}

void boot_mode_clear(void)
{
    boot_mode_enable_backup_domain();
    RTC->BKP0R = 0U;
    RTC->BKP1R = 0U;
}
