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

#include "MIN_at_TC74.h"



// Microchip TC74 I2C Sensor library



// Constructors ////////////////////////////////////////////////////////////////

TC74::TC74()
{
}



// Public Methods //////////////////////////////////////////////////////////////

void TC74::Begin()
{
	Begin(TC74A0_ADDR);
}

void TC74::Begin(byte address)
{
	Begin((int)address);
}

void TC74::Begin(int address)
{
	_addr = address;
}

void TC74::Standby(bool Value)
{
	uint8_t w = (Value ? 0x80 : 0x00);
	WriteConfigByte(w);
}

uint8_t TC74::ReadConfigByte()
{
	uint8_t temp;
	Tools::I2C_ReadByteDefault(_addr, TC74_CONFIG, I2C_SHORT_ADDR, &temp, 0x00);
	return temp;
}

void TC74::WriteConfigByte(uint8_t Value)
{
	Tools::I2C_WriteValue(_addr, TC74_CONFIG, I2C_SHORT_ADDR, Value, 0);
}

int8_t TC74::ReadTemperature()
{
	uint8_t temp;
	Tools::I2C_ReadByteDefault(_addr, TC74_TEMP, I2C_SHORT_ADDR, &temp, TC74_VALUE_ERROR);
	return (int8_t)temp;
}



// Private Methods //////////////////////////////////////////////////////////////



// Preinstantiate Objects //////////////////////////////////////////////////////

