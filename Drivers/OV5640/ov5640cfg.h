#ifndef _OV5640CFG_H
#define _OV5640CFG_H
#include "ov5640.h"
#include "ov5640_reg.h"

const uint8_t ov5640_exposure_table[7][6] =
{
	{ 0x10,0x08,0x10,0x08,0x20,0x10 },	// Exposure -3
	{ 0x20,0x18,0x41,0x20,0x18,0x10 },	// Exposure -2
	{ 0x30,0x28,0x61,0x30,0x28,0x10 },	// Exposure -1
	{ 0x38,0x30,0x61,0x38,0x30,0x10 },	// Exposure 0
	{ 0x40,0x38,0x71,0x40,0x38,0x10 },	// Exposure +1
	{ 0x50,0x48,0x90,0x50,0x48,0x20 },	// Exposure +2
	{ 0x60,0x58,0xa0,0x60,0x58,0x20 }	// Exposure +3
};

const uint8_t ov5640_light_mode_table[5][7] =
{
	{ 0x04,0X00,0X04,0X00,0X04,0X00,0X00 },	// Auto white balance
	{ 0x06,0X1C,0X04,0X00,0X04,0XF3,0X01 },	// Daylight white balance
	{ 0x05,0X48,0X04,0X00,0X07,0XCF,0X01 },	// Office lighting
	{ 0x06,0X48,0X04,0X00,0X04,0XD3,0X01 },	// Cloudy lighting
	{ 0x04,0X10,0X04,0X00,0X08,0X40,0X01 }	// Indoor lighting
};

const uint8_t ov5640_saturation_table[7][6] =
{
	{ 0X0C,0x30,0X3D,0X3E,0X3D,0X01 },	// Saturation -3
	{ 0X10,0x3D,0X4D,0X4E,0X4D,0X01 },	// Saturation -2
	{ 0X15,0x52,0X66,0X68,0X66,0X02 },	// Saturation -1
	{ 0X1A,0x66,0X80,0X82,0X80,0X02 },	// Saturation 0
	{ 0X1F,0x7A,0X9A,0X9C,0X9A,0X02 },	// Saturation +1
	{ 0X24,0x8F,0XB3,0XB6,0XB3,0X03 },	// Saturation +2
	{ 0X2B,0xAB,0XD6,0XDA,0XD6,0X04 }	// Saturation +3
};

const uint16_t ov5640_jpeg_init[] =
{
	OV5640_FORMAT_CTRL00, 0x30, // YUV422 YUYV output
	OV5640_FORMAT_MUX_CTRL, 0x00, // Route YUV422 to the output
	// Configure the sensor clock and frame timing.
	OV5640_SC_PLL_CONTRL1, 0x21, // PLL divider
	OV5640_SC_PLL_CONTRL2, 0x69, // PLL multiplier
	OV5640_LIGHTMETER1_TH_LOW, 0x07, // Light-meter threshold
	OV5640_TIMING_TC_REG20, 0x46, // Vertical flip
	OV5640_TIMING_TC_REG21, 0x20, // Horizontal mirror
	OV5640_TIMING_X_INC, 0x31, // Horizontal sampling
	OV5640_TIMING_Y_INC, 0x31, // Vertical sampling
	OV5640_TIMING_HS_HIGH, 0x00, // Crop start X high byte
	OV5640_TIMING_HS_LOW, 0x00, // Crop start X low byte
	OV5640_TIMING_VS_HIGH, 0x00, // Crop start Y high byte
	OV5640_TIMING_VS_LOW, 0x00, // Crop start Y low byte
	OV5640_TIMING_HW_HIGH, 0x05, // Crop end X high byte
	OV5640_TIMING_HW_LOW, 0x1f, // Crop end X low byte
	OV5640_TIMING_VH_HIGH, 0x03, // Crop end Y high byte
	OV5640_TIMING_VH_LOW, 0x5f, // Crop end Y low byte

	OV5640_TIMING_DVPHO_HIGH, 0x01, // Output width high byte
	OV5640_TIMING_DVPHO_LOW, 0x40, // Output width low byte
	OV5640_TIMING_DVPVO_HIGH, 0x00, // Output height high byte
	OV5640_TIMING_DVPVO_LOW, 0xf0, // Output height low byte

	OV5640_TIMING_HTS_HIGH, 0x05, // Horizontal total high byte
	OV5640_TIMING_HTS_LOW, 0x98, // Horizontal total low byte
	OV5640_TIMING_VTS_HIGH, 0x01, // Vertical total high byte
	OV5640_TIMING_VTS_LOW, 0xF0, // Vertical total low byte

	OV5640_TIMING_VOFFSET_LOW, 0x04, // Vertical timing offset
	OV5640_REG_3618, 0x00,
	OV5640_REG_3612, 0x29,
	OV5640_REG_3709, 0x52,
	OV5640_REG_370C, 0x03,

    OV5640_AEC_CTRL02, 0x02, // 60 Hz exposure high byte
	OV5640_AEC_CTRL03, 0xe0, // 60 Hz exposure low byte
	OV5640_AEC_MAX_EXPO_HIGH, 0x02, // 50 Hz exposure high byte
	OV5640_AEC_MAX_EXPO_LOW, 0xe0, // 50 Hz exposure low byte

	OV5640_BLC_CTRL04, 0x06, // Black-level correction lines
	OV5640_SYSREM_RESET02, 0x00, // Enable JPEG data paths
	OV5640_CLOCK_ENABLE02, 0xff, // Enable JPEG clocks
	OV5640_JPG_MODE_SELECT, 0x03, // Select JPEG mode
	OV5640_JPEG_CTRL07, 0x02, // JPEG quantization scale
	OV5640_REG_460B, 0x35,
	OV5640_VFIFO_CTRL0C, 0x22,
	OV5640_PCLK_PERIOD, 0x16, // MIPI clock period
	OV5640_REG_3824, 0x02, // Manual pixel-clock divider
	OV5640_ISP_CONTROL01, 0xA3, // Enable SDE, scaling, CMX, and AWB
	OV5640_AEC_PK_MANUAL, 0x00, // Enable automatic exposure and gain
};


const uint16_t ov5640_rgb565_init[]=
{
    OV5640_FORMAT_CTRL00, 0X6F,
	OV5640_FORMAT_MUX_CTRL, 0x01,
	// Configure the RGB565 output timing.
	// Input clock: 24 MHz; pixel clock: 42 MHz.
	OV5640_SC_PLL_CONTRL1, 0x21, // PLL
	OV5640_SC_PLL_CONTRL2, 0x69, // PLL
	OV5640_LIGHTMETER1_TH_LOW, 0x07, // Light-meter threshold
	OV5640_TIMING_TC_REG20, 0x46, // Vertical flip
	OV5640_TIMING_TC_REG21, 0x00, // Disable horizontal mirror
	OV5640_TIMING_X_INC, 0x31, // Horizontal sampling
	OV5640_TIMING_Y_INC, 0x31, // Vertical sampling
	OV5640_TIMING_HS_HIGH, 0x00, // Crop start X high byte
	OV5640_TIMING_HS_LOW, 0x00, // Crop start X low byte
	OV5640_TIMING_VS_HIGH, 0x00, // Crop start Y high byte
	OV5640_TIMING_VS_LOW, 0x00, // Crop start Y low byte
	OV5640_TIMING_HW_HIGH, 0x0a, // Crop end X high byte
	OV5640_TIMING_HW_LOW, 0x3f, // Crop end X low byte
	OV5640_TIMING_VH_HIGH, 0x06, // Crop end Y high byte
	OV5640_TIMING_VH_LOW, 0xa9, // Crop end Y low byte
	OV5640_TIMING_DVPHO_HIGH, 0x05, // Output width high byte
	OV5640_TIMING_DVPHO_LOW, 0x00, // Output width low byte
	OV5640_TIMING_DVPVO_HIGH, 0x02, // Output height high byte
	OV5640_TIMING_DVPVO_LOW, 0xd0, // Output height low byte
	OV5640_TIMING_HTS_HIGH, 0x05, // Horizontal total high byte
	OV5640_TIMING_HTS_LOW, 0xE8, // Horizontal total low byte
	OV5640_TIMING_VTS_HIGH, 0x03, // Vertical total high byte
	OV5640_TIMING_VTS_LOW, 0x84, // Vertical total low byte
	OV5640_TIMING_VOFFSET_LOW, 0x04, // Vertical timing offset
	OV5640_REG_3618, 0x00,
	OV5640_REG_3612, 0x29,
	OV5640_REG_3709, 0x52,
	OV5640_REG_370C, 0x03,
	OV5640_AEC_CTRL02, 0x02, // 60 Hz exposure high byte
	OV5640_AEC_CTRL03, 0xe0, // 60 Hz exposure low byte

	OV5640_AEC_MAX_EXPO_HIGH, 0x02, // 50 Hz exposure high byte
	OV5640_AEC_MAX_EXPO_LOW, 0xe0, // 50 Hz exposure low byte
	OV5640_BLC_CTRL04, 0x02, // Black-level correction lines
	OV5640_SYSREM_RESET02, 0x1c, // Reset JPEG data paths
	OV5640_CLOCK_ENABLE02, 0xc3, // Disable JPEG clocks
	OV5640_JPG_MODE_SELECT, 0x03, // Select JPEG mode
	OV5640_JPEG_CTRL07, 0x04, // JPEG quantization scale
	OV5640_REG_460B, 0x37,
	OV5640_VFIFO_CTRL0C, 0x20,
	OV5640_PCLK_PERIOD, 0x16, // MIPI clock period
	OV5640_REG_3824, 0x04, // Manual pixel-clock divider
	OV5640_ISP_CONTROL01, 0xA3, // Enable SDE, scaling, CMX, and AWB
	OV5640_AEC_PK_MANUAL, 0x00, // Enable automatic exposure and gain
};

const uint16_t ov5640_init_table[]=
{
	// Configure the sensor for a 24 MHz input and pixel clock.
	OV5640_SYSTEM_CTROL0, 0x42, // Enter software standby
	OV5640_SCCB_SYSTEM_CTRL1, 0x03, // Select PLL system clock
	OV5640_PAD_OUTPUT_ENABLE01, 0xff, // Enable sync, clock, and data outputs
	OV5640_PAD_OUTPUT_ENABLE02, 0xff, // Enable remaining data and GPIO outputs
	OV5640_SC_PLL_CONTRL0, 0x1a, // Select 10-bit MIPI mode
	OV5640_SC_PLL_CONTRL3, 0x13, // Configure PLL root and pre-dividers
	OV5640_SYSTEM_ROOT_DIVIDER, 0x01, // Configure pixel and system-clock dividers

	// Configure sensor analog and system control blocks.
	OV5640_REG_3630, 0x36,
	OV5640_REG_3631, 0x0e,
	OV5640_REG_3632, 0xe2,
	OV5640_REG_3633, 0x12,
	OV5640_REG_3621, 0xe0,
	OV5640_REG_3704, 0xa0,
	OV5640_REG_3703, 0x5a,
	OV5640_REG_3715, 0x78,
	OV5640_REG_3717, 0x01,
	OV5640_REG_370B, 0x60,
	OV5640_REG_3705, 0x1a,
	OV5640_REG_3905, 0x02,
	OV5640_REG_3906, 0x10,
	OV5640_REG_3901, 0x0a,
	OV5640_REG_3731, 0x12,
	OV5640_REG_3600, 0x08, // VCM control
	OV5640_REG_3601, 0x33, // VCM control
	OV5640_REG_302D, 0x60, // System control
	OV5640_REG_3620, 0x52,
	OV5640_REG_371B, 0x20,
	OV5640_REG_471C, 0x50,
	OV5640_AEC_CTRL13, 0x43, // Set pre-gain to 1.047x
	OV5640_AEC_GAIN_CEILING_HIGH, 0x00, // Gain ceiling high byte
	OV5640_AEC_GAIN_CEILING_LOW, 0xf8, // Set gain ceiling to 15.5x
	OV5640_REG_3635, 0x13,
	OV5640_REG_3636, 0x03,
	OV5640_REG_3634, 0x40,
	OV5640_REG_3622, 0x01,
	// Configure automatic 50/60 Hz light-flicker detection.
	OV5640_5060HZ_CTRL01, 0x34, // Enable automatic band detection
	OV5640_5060HZ_CTRL04, 0x28, // Set low-sum threshold
	OV5640_5060HZ_CTRL05, 0x98, // Set high-sum threshold
	OV5640_LIGHTMETER1_TH_HIGH, 0x00, // Light-meter 1 threshold high byte
	OV5640_LIGHTMETER1_TH_LOW, 0x08, // Light-meter 1 threshold low byte
	OV5640_LIGHTMETER2_TH_HIGH, 0x00, // Light-meter 2 threshold high byte
	OV5640_LIGHTMETER2_TH_LOW, 0x1c, // Light-meter 2 threshold low byte
	OV5640_SAMPLE_NUMBER_HIGH, 0x9c, // Sample count high byte
	OV5640_SAMPLE_NUMBER_LOW, 0x40, // Sample count low byte
	OV5640_TIMING_HOFFSET_HIGH, 0x00, // Horizontal offset high byte
	OV5640_TIMING_HOFFSET_LOW, 0x10, // Horizontal offset low byte
	OV5640_TIMING_VOFFSET_HIGH, 0x00, // Vertical offset high byte
	OV5640_REG_3708, 0x64,
	OV5640_BLC_CTRL01, 0x02, // Start black-level correction at line 2
	OV5640_BLC_CTRL05, 0x1a, // Enable continuous black-level updates
	OV5640_SYSREM_RESET00, 0x00, // Enable sensor processing blocks
	OV5640_CLOCK_ENABLE00, 0xff, // Enable sensor clocks
	OV5640_MIPI_CONTROL00, 0x58, // Power down MIPI and enable DVP
	OV5640_REG_302E, 0x00,
	OV5640_FORMAT_CTRL00, 0x30, // Set YUV422 YUYV format
	OV5640_FORMAT_MUX_CTRL, 0x00, // Route YUV422 to the output
	OV5640_REG_440E, 0x00,
	OV5640_ISP_CONTROL00, 0xa7, // Enable lens, gamma, BPC, WPC, and CIP
	// Configure automatic-exposure target ranges.
	OV5640_AEC_CTRL0F, 0x30, // Stable range in high byte
	OV5640_AEC_CTRL10, 0x28, // Stable range in low byte
	OV5640_AEC_CTRL1B, 0x30, // Stable range out high byte
	OV5640_AEC_CTRL1E, 0x26, // Stable range out low byte
	OV5640_AEC_CTRL11, 0x60, // Fast-zone high threshold
	OV5640_AEC_CTRL1F, 0x14, // Fast-zone low threshold
	// Configure lens-shading correction.
	OV5640_GMTRX00, 0x23,
	OV5640_GMTRX01, 0x14,
	OV5640_GMTRX02, 0x0f,
	OV5640_GMTRX03, 0x0f,
	OV5640_GMTRX04, 0x12,
	OV5640_GMTRX05, 0x26,
	OV5640_GMTRX10, 0x0c,
	OV5640_GMTRX11, 0x08,
	OV5640_GMTRX12, 0x05,
	OV5640_GMTRX13, 0x05,
	OV5640_GMTRX14, 0x08,

	OV5640_GMTRX15, 0x0d,
	OV5640_GMTRX20, 0x08,
	OV5640_GMTRX21, 0x03,
	OV5640_GMTRX22, 0x00,
	OV5640_GMTRX23, 0x00,
	OV5640_GMTRX24, 0x03,
	OV5640_GMTRX25, 0x09,
	OV5640_GMTRX30, 0x07,
	OV5640_GMTRX31, 0x03,
	OV5640_GMTRX32, 0x00,
	OV5640_GMTRX33, 0x01,
	OV5640_GMTRX34, 0x03,
	OV5640_GMTRX35, 0x08,
	OV5640_GMTRX40, 0x0d,
	OV5640_GMTRX41, 0x08,
	OV5640_GMTRX42, 0x05,
	OV5640_GMTRX43, 0x06,
	OV5640_GMTRX44, 0x08,
	OV5640_GMTRX45, 0x0e,
	OV5640_GMTRX50, 0x29,
	OV5640_GMTRX51, 0x17,
	OV5640_GMTRX52, 0x11,
	OV5640_GMTRX53, 0x11,
	OV5640_GMTRX54, 0x15,
	OV5640_GMTRX55, 0x28,
	OV5640_BRMATRX00, 0x46,
	OV5640_BRMATRX01, 0x26,
	OV5640_BRMATRX02, 0x08,
	OV5640_BRMATRX03, 0x26,
	OV5640_BRMATRX04, 0x64,
	OV5640_BRMATRX05, 0x26,
	OV5640_BRMATRX06, 0x24,
	OV5640_BRMATRX07, 0x22,
	OV5640_BRMATRX08, 0x24,
	OV5640_BRMATRX09, 0x24,
	OV5640_BRMATRX20, 0x06,
	OV5640_BRMATRX21, 0x22,
	OV5640_BRMATRX22, 0x40,
	OV5640_BRMATRX23, 0x42,
	OV5640_BRMATRX24, 0x24,
	OV5640_BRMATRX30, 0x26,
	OV5640_BRMATRX31, 0x24,
	OV5640_BRMATRX32, 0x22,
	OV5640_BRMATRX33, 0x22,
	OV5640_BRMATRX34, 0x26,
	OV5640_BRMATRX40, 0x44,
	OV5640_BRMATRX41, 0x24,
	OV5640_BRMATRX42, 0x26,
	OV5640_BRMATRX43, 0x28,
	OV5640_BRMATRX44, 0x42,
	OV5640_LENC_BR_OFFSET, 0xce, // Lens-shading blue/red offset
	// Configure automatic white balance.
	OV5640_AWB_CTRL00, 0xff, // Enable AWB B-channel correction
	OV5640_AWB_CTRL01, 0xf2, // AWB control
	OV5640_AWB_CTRL02, 0x00, // Local and fast-counter limits
	OV5640_AWB_CTRL03, 0x14, // Advanced AWB control
	OV5640_AWB_CTRL04, 0x25,
	OV5640_AWB_CTRL05, 0x24,
	OV5640_AWB_CTRL06, 0x09,
	OV5640_AWB_CTRL07, 0x09,
	OV5640_AWB_CTRL08, 0x09,
	OV5640_AWB_CTRL09, 0x75,
	OV5640_AWB_CTRL10, 0x54,
	OV5640_AWB_CTRL11, 0xe0,
	OV5640_AWB_CTRL12, 0xb2,
	OV5640_AWB_CTRL13, 0x42,
	OV5640_AWB_CTRL14, 0x3d,
	OV5640_AWB_CTRL15, 0x56,
	OV5640_AWB_CTRL16, 0x46,
	OV5640_AWB_CTRL17, 0xf8, // AWB upper limit
	OV5640_AWB_CTRL18, 0x04, // AWB lower limit
	OV5640_AWB_CTRL19, 0x70, // Red-channel limit
	OV5640_AWB_CTRL20, 0xf0, // Green-channel limit
	OV5640_AWB_CTRL21, 0xf0, // Blue-channel limit
	OV5640_AWB_CTRL22, 0x03, // AWB control
	OV5640_AWB_CTRL23, 0x01, // Local correction limit
	OV5640_AWB_CTRL24, 0x04,
	OV5640_AWB_CTRL25, 0x12,
	OV5640_AWB_CTRL26, 0x04,
	OV5640_AWB_CTRL27, 0x00,
	OV5640_AWB_CTRL28, 0x06,
	OV5640_AWB_CTRL29, 0x82,
	OV5640_AWB_CTRL30, 0x38, // AWB control
	// Configure the luminance gamma curve.
	OV5640_GAMMA_CTRL00, 0x01, // Enable positive gamma bias
	OV5640_GAMMA_YST00, 0x08,
	OV5640_GAMMA_YST01, 0x14,
	OV5640_GAMMA_YST02, 0x28,
	OV5640_GAMMA_YST03, 0x51,
	OV5640_GAMMA_YST04, 0x65,
	OV5640_GAMMA_YST05, 0x71,
	OV5640_GAMMA_YST06, 0x7d,
	OV5640_GAMMA_YST07, 0x87,
	OV5640_GAMMA_YST08, 0x91,
	OV5640_GAMMA_YST09, 0x9a,
	OV5640_GAMMA_YST0A, 0xaa,
	OV5640_GAMMA_YST0B, 0xb8,
	OV5640_GAMMA_YST0C, 0xcd,
	OV5640_GAMMA_YST0D, 0xdd,
	OV5640_GAMMA_YST0E, 0xea,
	OV5640_GAMMA_YST0F, 0x1d,
	// Configure the YUV color-conversion matrix.
	OV5640_CMX1, 0x1e, // Luma coefficient 1
	OV5640_CMX2, 0x5b, // Luma coefficient 2
	OV5640_CMX3, 0x08, // Luma coefficient 3
	OV5640_CMX4, 0x0a, // Chroma-U coefficient 1
	OV5640_CMX5, 0x7e, // Chroma-U coefficient 2
	OV5640_CMX6, 0x88, // Chroma-U coefficient 3
	OV5640_CMX7, 0x7c, // Chroma-V coefficient 1
	OV5640_CMX8, 0x6c, // Chroma-V coefficient 2
	OV5640_CMX9, 0x10, // Chroma-V coefficient 3
	OV5640_CMXSIGN_HIGH, 0x01, // Matrix sign bits high
	OV5640_CMXSIGN_LOW, 0x98, // Matrix sign bits low
	// Configure saturation and UV offsets.
	OV5640_SDE_CTRL0, 0x06, // Enable saturation control
	OV5640_SDE_CTRL3, 0x40,
	OV5640_SDE_CTRL4, 0x10,
	OV5640_SDE_CTRL9, 0x10,
	OV5640_SDE_CTRL10, 0x00,
	OV5640_SDE_CTRL11, 0xf8,
	OV5640_ISP_MISC0, 0x40, // Enable manual contrast offset
	// Configure color-interpolation sharpening and denoising.
	OV5640_CIP_SHARPENMT_TH1, 0x08, // Medium-tone sharpen threshold 1
	OV5640_CIP_SHARPENMT_TH2, 0x30, // Medium-tone sharpen threshold 2
	OV5640_CIP_SHARPENMT_OFFSET1, 0x10, // Medium-tone sharpen offset 1
	OV5640_CIP_SHARPENMT_OFFSET2, 0x00, // Medium-tone sharpen offset 2
	OV5640_CIP_DNS_TH1, 0x08, // Denoise threshold 1
	OV5640_CIP_DNS_TH2, 0x30, // Denoise threshold 2
	OV5640_CIP_DNS_OFFSET1, 0x08, // Denoise offset 1
	OV5640_CIP_DNS_OFFSET2, 0x16, // Denoise offset 2
	OV5640_CIP_SHARPENTH_TH1, 0x08, // High-tone sharpen threshold 1
	OV5640_CIP_SHARPENTH_TH2, 0x30, // High-tone sharpen threshold 2
	OV5640_CIP_SHARPENTH_OFFSET1, 0x04, // High-tone sharpen offset 1
	OV5640_CIP_SHARPENTH_OFFSET2, 0x06, // High-tone sharpen offset 2
	OV5640_REG_5025, 0x00,
	OV5640_SYSTEM_CTROL0, 0x02, // Exit software standby

	OV5640_POLARITY_CTRL, 0X21, // Set VSYNC active high

};

#endif











