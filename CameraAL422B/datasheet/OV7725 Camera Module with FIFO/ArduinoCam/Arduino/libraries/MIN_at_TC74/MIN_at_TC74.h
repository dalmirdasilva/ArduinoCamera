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

#ifndef MIN_at_TC74_h
#define MIN_at_TC74_h

#include <inttypes.h>
#include <Arduino.h>
#include "MIN_at_Tools.h"


#define TC74A0_ADDR				0x48
#define TC74A1_ADDR				0x49
#define TC74A2_ADDR				0x4A
#define TC74A3_ADDR				0x4B
#define TC74A4_ADDR				0x4C
#define TC74A5_ADDR				0x4D
#define TC74A6_ADDR				0x4E
#define TC74A7_ADDR				0x4F

#define TC74_TEMP				0x00
#define TC74_CONFIG				0x01
#define TC74_VALUE_ERROR		0x80


class TC74
{
  private:
	int _addr;
	
  
  public:
    TC74();
	
	void Begin();
	void Begin(uint8_t address);
	void Begin(int address);
	
	void Standby(bool Value);
	
	uint8_t ReadConfigByte();
	void WriteConfigByte(uint8_t value);
	int8_t ReadTemperature();
};

#endif
