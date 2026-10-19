#ifndef _OV5640_H
#define _OV5640_H

#include "main.h"


#define  OV5640_PWDN_Pin_RESET   HAL_GPIO_WritePin(OV5640_PWDN_GPIO_Port, OV5640_PWDN_Pin, GPIO_PIN_RESET)


#define OV5640_ADDR        	    0X78		
#define OV5640_CHIPIDH          0X300A  	
#define OV5640_CHIPIDL          0X300B  	

#define   USER_KEY_PRESS        HAL_GPIO_ReadPin(USER_KEY_GPIO_Port, USER_KEY_Pin) == 0
#define   WAKE_UP_KEY_PRESS     HAL_GPIO_ReadPin(WAKE_UP_KEY_GPIO_Port, WAKE_UP_KEY_Pin) == 0

#define   XSIZE      320
#define   YSIZE      240


#define   QQVGA_160_120    0
#define   QCIF_176_144     1
#define   QVGA_320_240     2
#define   WQVGA_400_240    3
#define   CIF_352_288      4


uint8_t ov5640_wr_reg(uint16_t reg, uint8_t data);
uint8_t ov5640_rd_reg(uint16_t reg);
uint8_t ov5640_init(void);
void ov5640_jpeg_mode(void);
void ov5640_rgb565_mode(void);
void ov5640_exposure(uint8_t exposure);
void ov5640_light_mode(uint8_t mode);
void ov5640_color_saturation(uint8_t sat);
void ov5640_brightness(uint8_t bright);
void ov5640_contrast(uint8_t contrast);
void ov5640_sharpness(uint8_t sharp);
void ov5640_special_effects(uint8_t eft);
void ov5640_test_pattern(uint8_t mode);
void ov5640_flash_lamp(uint8_t sw);
uint8_t ov5640_out_size_set(uint16_t offx, uint16_t offy, uint16_t width, uint16_t height);
uint8_t ov5640_focus_init(void);
uint8_t ov5640_auto_focus(void);

#endif




















