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

#ifndef Tools_h
#define Tools_h

#include <inttypes.h>
#include <Arduino.h>
#include <HardwareSerial.h>
#include "../Wire/Wire.h"
#include "../DigitalWriteFast/DigitalWriteFast.h"

#define CHAR_BACKSPACE			0x08
#define CHAR_BELL				0x07
#define CHAR_CR					0x0D
#define CHAR_LF					0x0A

#define I2C_SHORT_ADDR			0
#define I2C_LONG_ADDR			1

#define DI_ALLOW_CR				1
#define DI_ECHO					2
#define DI_AUTO_SKIP			4


class Tools
{
  public:
    Tools();
	
	static int ReadDec(uint8_t MaxLen, uint8_t Flags, bool *Valid);
	
	static char *FormatHEX(uint8_t Value, uint8_t Prefix);
	static char *FormatHEX16(int Value, uint8_t Prefix);
	static char *FormatBIN(uint8_t Value);
	
	static void I2C_Write(uint8_t I2cAddr, uint16_t MemAddr, uint8_t UseLongAddr);
	static void I2C_WriteValue(uint8_t I2cAddr, uint16_t MemAddr, uint8_t UseLongAddr,
								uint8_t Value, int Delay);
					
	static bool I2C_ReadByte(uint8_t I2cAddr, uint16_t MemAddr, uint8_t UseLongAddr,
								uint8_t *Value);	
	static bool I2C_ReadByteDefault(uint8_t I2cAddr, uint16_t MemAddr, uint8_t UseLongAddr,
									uint8_t *Value, uint8_t DefaultValue);
					
	static void I2C_SetBitAt(uint8_t I2cAddr, uint16_t MemAddr, uint8_t UseLongAddr,
								uint8_t BitNum, bool Value, int Delay);

	static void I2C_EEWriteBuffer(uint8_t I2cAddr, uint16_t MemAddr, 
									byte *Data, byte Length);
	static void I2C_EEReadBuffer(uint8_t I2cAddr, uint16_t MemAddr, 
									byte *Data, int Length);

								
	static uint8_t bcdToDec(uint8_t Value);
	static uint8_t dec2bcd(uint8_t num);
};

#endif

