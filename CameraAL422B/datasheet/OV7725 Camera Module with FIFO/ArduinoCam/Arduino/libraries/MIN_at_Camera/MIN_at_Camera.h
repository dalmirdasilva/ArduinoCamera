// ArduinoCam Version 1.0 (2012-09-01) Firmware (Duemilanove Atmega168)
// Copyright 2012 richard.prinz@min.at
// See http://www.min.at/prinz/oe1rib/ArduinoCam/ for more infos
//
// This file is part of ArduinoCam
//
// ArduinoCam is free software and hardware design:
// you can redistribute the software and the hardware design and/or modify it under 
// the terms of the GNU General Public License as published by the Free Software Foundation, 
// either version 3 of the License, or (at your option) any later version.
//
// ArduinoCam is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ArduinoCam. 
// If not, see http://www.gnu.org/licenses/.

#ifndef MIN_at_Camera_h
#define MIN_at_Camera_h

#include <inttypes.h>
#include <Arduino.h>
#include "MIN_at_Tools.h"


#define  OV_7620_ADDR            0x21

#define  OV_AGC                  0x00
#define  OV_BLUE_GAIN            0x01
#define  OV_RED_GAIN             0x02
#define  OV_SATURATION           0x03

#define  OV_BRIGHTNESS           0x06
#define  OV_ANALOG_SHARPNESS     0x07

#define  OV_WBAL_BLUE            0x0C
#define  OV_WBAL_RED             0x0D

#define  OV_AUTO_EXPOSURE        0x10
#define  OV_CLOCK_RATE           0x11
#define  OV_COMMON_A             0x12
#define  OV_COMMON_B             0x13
#define  OV_COMMON_C             0x14
#define  OV_COMMON_D             0x15
#define  OV_FRAME_DROP           0x16
#define  OV_HWIN_START           0x17
#define  OV_HWIN_END             0x18
#define  OV_VWIN_START           0x19
#define  OV_VWIN_END             0x1A
#define  OV_PIXEL_SHIFT          0x1B
#define  OV_ID_H                 0x1C
#define  OV_ID_L                 0x1D

#define  OV_COMMON_E             0x20
#define  OV_YCHAN_OFFSET         0x21
#define  OV_UCHAN_OFFSET         0x22
#define  OV_CRYSTAL_CURRENT      0x23
#define  OV_AEW_PIXEL_RATIO      0x24
#define  OV_AEB_PIXEL_RATIO      0x25
#define  OV_COMMON_F             0x26
#define  OV_COMMON_G             0x27
#define  OV_COMMON_H             0x28
#define  OV_COMMON_I             0x29
#define  OV_FRAME_RATE_1         0x2A
#define  OV_FRAME_RATE_2         0x2B
#define  OV_BLACK_EXPAND         0x2C
#define  OV_COMMON_J             0x2D
#define  OV_VCHAN_OFFSET         0x2E

#define  OV_SIGNAL_A             0x60
#define  OV_SIGNAL_B             0x61
#define  OV_RGB_GAMMA            0x62

#define  OV_Y_GAMMA              0x64
#define  OV_SIGNAL_C             0x65
#define  OV_AWB_CONTROL          0x66
#define  OV_COLOR_SPACE          0x67
#define  OV_SIGNAL_D             0x68
#define  OV_HEDGE_ENH            0x69
#define  OV_VEDGE_ENH            0x6A

#define  OV_E_O_NOISE            0x6F
#define  OV_COMMON_K             0x70
#define  OV_COMMON_L             0x71
#define  OV_HSYNC_EDGE_1         0x72
#define  OV_HSYNC_EDGE_2         0x73
#define  OV_COMMON_M             0x74
#define  OV_COMMON_N             0x75
#define  OV_COMMON_O             0x76

#define  OV_FIELD_AVG            0x7C

/*
#define  BUS_0                   0x02
#define  BUS_1                   0x03
#define  BUS_2                   0x02
#define  BUS_3                   0x03
#define  BUS_4                   0x04
#define  BUS_5                   0x05
#define  BUS_6                   0x06
#define  BUS_7                   0x07
*/

#define  BUS_RRST                0x08
#define  BUS_RCK                 0x09

class Camera
{
  private:
	int _addr;
	
	static uint8_t Clip(float Value);
	
  
  public:
    Camera();
	
	void Begin();
	void Begin(uint8_t address);
	void Begin(int address);
	
	bool Reset();
	
	void Init();
	void ColorBar(bool On);
	void Power(bool On);
	void Mirror(bool On);
	bool Capture();
	void Dump(bool Hex);
	void DumpConfig();

	uint8_t ReadConfigByte(uint8_t MemAddr);	
	uint8_t ReadNextVideoByte();
	void DumpVideoByte(uint8_t pixel, uint8_t *count);
	void ResetVideoPointer();
	
	static void DebugPrintValue(uint8_t Value);
	static void UYV2RGB(uint8_t U, uint8_t Y, uint8_t V, uint8_t *R, uint8_t *G, uint8_t *B);
};

extern Camera Cam;

#endif

