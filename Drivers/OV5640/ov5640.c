/**
  ******************************************************************************
  * @file    ov5640.c
  * @brief   Provide driver code for OV5640 camera
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 Dang Cao Minh <dangcaominhheo@gmail.com>
  * SPDX-License-Identifier: Apache-2.0
  *
  ******************************************************************************
  */

#include <stdio.h>
#include <stdint.h>
#include "main.h"
#include "ov5640.h"
#include "ov5640cfg.h"
#include "ov5640af.h"			 	

extern I2C_HandleTypeDef hi2c1;

/**
  * @brief  Ghi 1 byte vào thanh ghi OV5640 thông qua Hardware I2C1
  * @param  reg: Địa chỉ thanh ghi (16-bit)
  * @param  data: Dữ liệu ghi
  * @retval 0: Thành công, 1: Thất bại
  */
uint8_t ov5640_wr_reg(uint16_t reg, uint8_t data)
{
	if (HAL_I2C_Mem_Write(&hi2c1, OV5640_ADDR, reg, I2C_MEMADD_SIZE_16BIT, &data, 1, 100) == HAL_OK)
	{
		return 0; // OK
	}
	return 1; // Lỗi truyền dữ liệu
}

/**
  * @brief  Đọc 1 byte từ thanh ghi OV5640 thông qua Hardware I2C1
  * @param  reg: Địa chỉ thanh ghi (16-bit)
  * @retval Giá trị đọc được từ thanh ghi
  */
uint8_t ov5640_rd_reg(uint16_t reg)
{
	uint8_t val = 0;
	HAL_I2C_Mem_Read(&hi2c1, OV5640_ADDR, reg, I2C_MEMADD_SIZE_16BIT, &val, 1, 100);
	return val;
}

uint8_t ov5640_init(void)
{
	uint16_t i = 0;
	uint16_t reg;

	reg = ov5640_rd_reg(OV5640_CHIPIDH);
	reg <<= 8;
	reg |= ov5640_rd_reg(OV5640_CHIPIDL);
	if (reg != OV5640_ID)
	{
		printf("ID: %d \r\n", reg);
		return 1;
	}

	for (i = 0; i < sizeof(ov5640_init_table) / (2 * sizeof(ov5640_init_table[0])); i++)
	{
		ov5640_wr_reg(ov5640_init_table[i * 2], ov5640_init_table[i * 2 + 1]);
	}

	return 0;
}

void ov5640_jpeg_mode(void)
{
	uint16_t i = 0;

	for (i = 0; i < sizeof(ov5640_jpeg_init) / (2 * sizeof(ov5640_jpeg_init[0])); i++)
	{
		ov5640_wr_reg(ov5640_jpeg_init[i * 2], ov5640_jpeg_init[i * 2 + 1]);
	}
}

void ov5640_rgb565_mode(void)
{
	uint16_t i = 0;

	for (i = 0; i < sizeof(ov5640_rgb565_init) / (2 * sizeof(ov5640_rgb565_init[0])); i++)
	{
		ov5640_wr_reg(ov5640_rgb565_init[i * 2], ov5640_rgb565_init[i * 2 + 1]);
	}
}

void ov5640_exposure(uint8_t exposure)
{
	ov5640_wr_reg(0x3212, 0x03);	//start group 3
	ov5640_wr_reg(0x3a0f, ov5640_exposure_table[exposure][0]);
	ov5640_wr_reg(0x3a10, ov5640_exposure_table[exposure][1]);
	ov5640_wr_reg(0x3a1b, ov5640_exposure_table[exposure][2]);
	ov5640_wr_reg(0x3a1e, ov5640_exposure_table[exposure][3]);
	ov5640_wr_reg(0x3a11, ov5640_exposure_table[exposure][4]);
	ov5640_wr_reg(0x3a1f, ov5640_exposure_table[exposure][5]);
	ov5640_wr_reg(0x3212, 0x13); //end group 3
	ov5640_wr_reg(0x3212, 0xa3); //launch group 3
}

void ov5640_light_mode(uint8_t mode)
{
	ov5640_wr_reg(0x3212, 0x03);	//start group 3
	for (int i = 0; i < 7; i++)
	{
		ov5640_wr_reg(0x3400 + i, ov5640_light_mode_table[mode][i]);
	}
	ov5640_wr_reg(0x3212, 0x13); //end group 3
	ov5640_wr_reg(0x3212, 0xa3); //launch group 3	
}

void ov5640_color_saturation(uint8_t sat)
{
	uint8_t i;
	ov5640_wr_reg(0x3212, 0x03);	//start group 3
	ov5640_wr_reg(0x5381, 0x1c);
	ov5640_wr_reg(0x5382, 0x5a);
	ov5640_wr_reg(0x5383, 0x06);
	for (i = 0; i < 6; i++)
	{
		ov5640_wr_reg(0x5384 + i, ov5640_saturation_table[sat][i]);
	}
	ov5640_wr_reg(0x538b, 0x98);
	ov5640_wr_reg(0x538a, 0x01);
	ov5640_wr_reg(0x3212, 0x13); //end group 3
	ov5640_wr_reg(0x3212, 0xa3); //launch group 3	
}

void ov5640_brightness(uint8_t bright)
{
	uint8_t brtval;
	if (bright < 4)
	{
		brtval = 4 - bright;
	}
	else
	{
		brtval = bright - 4;
	}
	ov5640_wr_reg(0x3212, 0x03);	//start group 3
	ov5640_wr_reg(0x5587, brtval << 4);
	if (bright < 4)
	{
		ov5640_wr_reg(0x5588, 0x09);
	}
	else
	{
		ov5640_wr_reg(0x5588, 0x01);
	}
	ov5640_wr_reg(0x3212, 0x13); //end group 3
	ov5640_wr_reg(0x3212, 0xa3); //launch group 3
}

void ov5640_contrast(uint8_t contrast)
{
	uint8_t reg0val = 0X00;
	uint8_t reg1val = 0X20;
	switch (contrast)
	{
	case 0://-3
		reg1val = reg0val = 0X14;
		break;
	case 1://-2
		reg1val = reg0val = 0X18;
		break;
	case 2://-1
		reg1val = reg0val = 0X1C;
		break;
	case 4://1
		reg0val = 0X10;
		reg1val = 0X24;
		break;
	case 5://2
		reg0val = 0X18;
		reg1val = 0X28;
		break;
	case 6://3
		reg0val = 0X1C;
		reg1val = 0X2C;
		break;
	}
	ov5640_wr_reg(0x3212, 0x03); //start group 3
	ov5640_wr_reg(0x5585, reg0val);
	ov5640_wr_reg(0x5586, reg1val);
	ov5640_wr_reg(0x3212, 0x13); //end group 3
	ov5640_wr_reg(0x3212, 0xa3); //launch group 3
}

void ov5640_sharpness(uint8_t sharp)
{
	if (sharp < 33)
	{
		ov5640_wr_reg(0x5308, 0x65);
		ov5640_wr_reg(0x5302, sharp);
	}
	else	// auto
	{
		ov5640_wr_reg(0x5308, 0x25);
		ov5640_wr_reg(0x5300, 0x08);
		ov5640_wr_reg(0x5301, 0x30);
		ov5640_wr_reg(0x5302, 0x10);
		ov5640_wr_reg(0x5303, 0x00);
		ov5640_wr_reg(0x5309, 0x08);
		ov5640_wr_reg(0x530a, 0x30);
		ov5640_wr_reg(0x530b, 0x04);
		ov5640_wr_reg(0x530c, 0x06);
	}
}

static const uint8_t OV5640_EFFECTS_TBL[7][3] =
{
	{ 0X06,0x40,0X10 }, // normal
	{ 0X1E,0xA0,0X40 },
	{ 0X1E,0x80,0XC0 },
	{ 0X1E,0x80,0X80 },
	{ 0X1E,0x40,0XA0 },
	{ 0X40,0x40,0X10 },
	{ 0X1E,0x60,0X60 }
};

void ov5640_special_effects(uint8_t eft)
{
	ov5640_wr_reg(0x3212, 0x03); //start group 3
	ov5640_wr_reg(0x5580, OV5640_EFFECTS_TBL[eft][0]);
	ov5640_wr_reg(0x5583, OV5640_EFFECTS_TBL[eft][1]);// sat U
	ov5640_wr_reg(0x5584, OV5640_EFFECTS_TBL[eft][2]);// sat V
	ov5640_wr_reg(0x5003, 0x08);
	ov5640_wr_reg(0x3212, 0x13); //end group 3
	ov5640_wr_reg(0x3212, 0xa3); //launch group 3
}

void ov5640_flash_lamp(uint8_t sw)
{
	ov5640_wr_reg(0x3016, 0X02);
	ov5640_wr_reg(0x301C, 0X02);
	if (sw)
	{
		ov5640_wr_reg(0X3019, 0X02);
	}
	else
	{
		ov5640_wr_reg(0X3019, 0X00);
	}
}

uint8_t ov5640_out_size_set(uint16_t offx, uint16_t offy, uint16_t width, uint16_t height)
{
	ov5640_wr_reg(0X3212, 0X03);

	ov5640_wr_reg(0x3808, width >> 8);
	ov5640_wr_reg(0x3809, width & 0xff);
	ov5640_wr_reg(0x380a, height >> 8);
	ov5640_wr_reg(0x380b, height & 0xff);

	ov5640_wr_reg(0x3810, offx >> 8);
	ov5640_wr_reg(0x3811, offx & 0xff);

	ov5640_wr_reg(0x3812, offy >> 8);
	ov5640_wr_reg(0x3813, offy & 0xff);

	ov5640_wr_reg(0X3212, 0X13);
	ov5640_wr_reg(0X3212, 0Xa3);

	return 0;
}

uint8_t ov5640_focus_init(void)
{
	uint16_t i;
	uint16_t addr = 0x8000;
	uint8_t state = 0x8F;
	ov5640_wr_reg(0x3000, 0x20);	//reset 	 
	for (i = 0;i < sizeof(OV5640_AF_Config);i++)
	{
		ov5640_wr_reg(addr, OV5640_AF_Config[i]);
		addr++;
	}
	ov5640_wr_reg(0x3022, 0x00);
	ov5640_wr_reg(0x3023, 0x00);
	ov5640_wr_reg(0x3024, 0x00);
	ov5640_wr_reg(0x3025, 0x00);
	ov5640_wr_reg(0x3026, 0x00);
	ov5640_wr_reg(0x3027, 0x00);
	ov5640_wr_reg(0x3028, 0x00);
	ov5640_wr_reg(0x3029, 0x7f);
	ov5640_wr_reg(0x3000, 0x00);
	i = 0;
	do
	{
		state = ov5640_rd_reg(0x3029);
		HAL_Delay(5);
		i++;
		if (i > 1000)
		{
			return 1;
		}
	} while (state != 0x70);
	return 0;
}

uint8_t ov5640_auto_focus(void)
{
	uint8_t temp = 0;
	uint16_t retry = 0;
	ov5640_wr_reg(0x3023, 0x01);
	ov5640_wr_reg(0x3022, 0x08);
	do
	{
		temp = ov5640_rd_reg(0x3023);
		retry++;
		if (retry > 1000)
		{
			return 2;
		}
		HAL_Delay(5);
	} while (temp != 0x00);
	ov5640_wr_reg(0x3023, 0x01);
	ov5640_wr_reg(0x3022, 0x04);
	retry = 0;
	do
	{
		temp = ov5640_rd_reg(0x3023);
		retry++;
		if (retry > 1000)
		{
			return 2;
		}
		HAL_Delay(5);
	} while (temp != 0x00);
	return 0;
}