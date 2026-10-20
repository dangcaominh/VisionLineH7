/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_dfu_if.c
  * @brief          : Usb device for Download Firmware Update.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbd_dfu_if.h"

/* USER CODE BEGIN INCLUDE */
#include "main.h"
#include "stm32h7xx_hal_flash_ex.h"
#include <string.h>
/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device.
  * @{
  */

/** @defgroup USBD_DFU
  * @brief Usb DFU device module.
  * @{
  */

/** @defgroup USBD_DFU_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_DFU_Private_Defines
  * @brief Private defines.
  * @{
  */

#define FLASH_DESC_STR      "@DFU STM32/0x08020000/07*128Kg,08*128Kg"
/* USER CODE BEGIN PRIVATE_DEFINES */
#define APP_FLASH_START     0x08020000UL
#define APP_FLASH_END       0x08200000UL
#define FLASH_BANK2_START   0x08100000UL

static uint8_t IsAppFlashRange(uint32_t address, uint32_t length)
{
  if ((address < APP_FLASH_START) || (address >= APP_FLASH_END))
  {
    return 0U;
  }

  return (length <= (APP_FLASH_END - address)) ? 1U : 0U;
}
/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_DFU_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_DFU_Private_Variables
  * @brief Private variables.
  * @{
  */

/* USER CODE BEGIN PRIVATE_VARIABLES */

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_DFU_Exported_Variables
  * @brief Public variables.
  * @{
  */

extern USBD_HandleTypeDef hUsbDeviceHS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_DFU_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static uint16_t MEM_If_Init_HS(void);
static uint16_t MEM_If_Erase_HS(uint32_t Add);
static uint16_t MEM_If_Write_HS(uint8_t *src, uint8_t *dest, uint32_t Len);
static uint8_t *MEM_If_Read_HS(uint8_t *src, uint8_t *dest, uint32_t Len);
static uint16_t MEM_If_DeInit_HS(void);
static uint16_t MEM_If_GetStatus_HS(uint32_t Add, uint8_t Cmd, uint8_t *buffer);

/* USER CODE BEGIN PRIVATE_FUNCTIONS_DECLARATION */

/* USER CODE END PRIVATE_FUNCTIONS_DECLARATION */

/**
  * @}
  */

#if defined ( __ICCARM__ ) /* IAR Compiler */
  #pragma data_alignment=4
#endif

__ALIGN_BEGIN USBD_DFU_MediaTypeDef USBD_DFU_fops_HS __ALIGN_END =
{
    (uint8_t*)FLASH_DESC_STR,
    MEM_If_Init_HS,
    MEM_If_DeInit_HS,
    MEM_If_Erase_HS,
    MEM_If_Write_HS,
    MEM_If_Read_HS,
    MEM_If_GetStatus_HS
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Memory initialization routine.
  * @retval USBD_OK if operation is successful, MAL_FAIL else.
  */
uint16_t MEM_If_Init_HS(void)
{
  /* USER CODE BEGIN 6 */
  return (HAL_FLASH_Unlock() == HAL_OK) ? USBD_OK : USBD_FAIL;
  /* USER CODE END 6 */
}

/**
  * @brief  De-Initializes Memory.
  * @retval USBD_OK if operation is successful, MAL_FAIL else.
  */
uint16_t MEM_If_DeInit_HS(void)
{
  /* USER CODE BEGIN 7 */
  return (HAL_FLASH_Lock() == HAL_OK) ? USBD_OK : USBD_FAIL;
  /* USER CODE END 7 */
}

/**
  * @brief  Erase sector.
  * @param  Add: Address of sector to be erased.
  * @retval USBD_OK if operation is successful, MAL_FAIL else.
  */
uint16_t MEM_If_Erase_HS(uint32_t Add)
{
  /* USER CODE BEGIN 8 */
  FLASH_EraseInitTypeDef erase_init = {0};
  uint32_t sector_error = 0U;
  uint32_t sector;
  uint32_t bank;

  if (IsAppFlashRange(Add, 1U) == 0U)
  {
    return USBD_FAIL;
  }

  erase_init.TypeErase = FLASH_TYPEERASE_SECTORS;
  if (Add < FLASH_BANK2_START)
  {
    bank = FLASH_BANK_1;
    sector = (Add - FLASH_BANK1_BASE) / 0x00020000UL;
  }
  else
  {
    bank = FLASH_BANK_2;
    sector = (Add - FLASH_BANK2_START) / 0x00020000UL;
  }

  erase_init.Banks = bank;
  erase_init.Sector = sector;
  erase_init.NbSectors = 1U;
  erase_init.VoltageRange = FLASH_VOLTAGE_RANGE_3;

  return (HAL_FLASHEx_Erase(&erase_init, &sector_error) == HAL_OK) ? USBD_OK : USBD_FAIL;
  /* USER CODE END 8 */
}

/**
  * @brief  Memory write routine.
  * @param  src: Pointer to the source buffer. Address to be written to.
  * @param  dest: Pointer to the destination buffer.
  * @param  Len: Number of data to be written (in bytes).
  * @retval USBD_OK if operation is successful, MAL_FAIL else.
  */
uint16_t MEM_If_Write_HS(uint8_t *src, uint8_t *dest, uint32_t Len)
{
  /* USER CODE BEGIN 9 */
  uint32_t address = (uint32_t)dest;
  uint32_t offset;

  if ((src == NULL) || (dest == NULL) || (IsAppFlashRange(address, Len) == 0U) ||
      ((address & 0x1FU) != 0U))
  {
    return USBD_FAIL;
  }

  for (offset = 0U; offset < Len; offset += 32U)
  {
    uint32_t flash_word[8];
    uint32_t remaining = Len - offset;
    uint32_t copy_length = (remaining < sizeof(flash_word)) ? remaining : sizeof(flash_word);

    memset(flash_word, 0xFF, sizeof(flash_word));
    memcpy(flash_word, &src[offset], copy_length);

    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD,
                          address + offset,
                          (uint32_t)flash_word) != HAL_OK)
    {
      return USBD_FAIL;
    }
  }

  return USBD_OK;
  /* USER CODE END 9 */
}

/**
  * @brief  Memory read routine.
  * @param  src: Pointer to the source buffer. Address to be written to.
  * @param  dest: Pointer to the destination buffer.
  * @param  Len: Number of data to be read (in bytes).
  * @retval Pointer to the physical address where data should be read.
  */
uint8_t *MEM_If_Read_HS(uint8_t *src, uint8_t *dest, uint32_t Len)
{
  /* Return a valid address to avoid HardFault */
  /* USER CODE BEGIN 10 */
  UNUSED(dest);

  return (IsAppFlashRange((uint32_t)src, Len) != 0U) ? src : NULL;
  /* USER CODE END 10 */
}

/**
  * @brief  Get status routine.
  * @param  Add: Address to be read from.
  * @param  Cmd: Number of data to be read (in bytes).
  * @param  buffer: used for returning the time necessary for a program or an erase operation
  * @retval 0 if operation is successful
  */
uint16_t MEM_If_GetStatus_HS(uint32_t Add, uint8_t Cmd, uint8_t *buffer)
{
  /* USER CODE BEGIN 11 */
  UNUSED(Add);

  if (buffer == NULL)
  {
    return USBD_FAIL;
  }

  switch (Cmd)
  {
    case DFU_MEDIA_PROGRAM:
      buffer[1] = 1U;
      buffer[2] = 0U;
      buffer[3] = 0U;
      break;

    case DFU_MEDIA_ERASE:
      buffer[1] = 25U;
      buffer[2] = 0U;
      buffer[3] = 0U;
      break;

    default:
      return USBD_FAIL;
  }

  return USBD_OK;
  /* USER CODE END 11 */
}

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */

/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */

/**
  * @}
  */

/**
  * @}
  */
