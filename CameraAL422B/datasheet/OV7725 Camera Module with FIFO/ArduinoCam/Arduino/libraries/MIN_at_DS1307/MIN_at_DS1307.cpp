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

#include "MIN_at_DS1307.h"



// Microchip DS1307 RTC



// Constructors ////////////////////////////////////////////////////////////////

DS1307::DS1307()
{
}



// Public Methods //////////////////////////////////////////////////////////////

void DS1307::Begin()
{
	Begin(DS1307_ADDR);
}

void DS1307::Begin(byte address)
{
	Begin((int)address);
}

void DS1307::Begin(int address)
{
	_addr = address;
}

void DS1307::Reset()
{
	Wire.beginTransmission(_addr);
	for(int i = 0; i < 8; i++)	
		Wire.write((uint8_t)0x00);
	Wire.endTransmission();
}

uint8_t DS1307::ReadConfigByte()
{
	uint8_t temp;
	Tools::I2C_ReadByteDefault(_addr, DS1307_CONFIG, I2C_SHORT_ADDR, &temp, 0x00);
	return temp;
}

void DS1307::WriteConfigByte(uint8_t Value)
{
	Tools::I2C_WriteValue(_addr, DS1307_CONFIG, I2C_SHORT_ADDR, Value, 10);
}

bool DS1307::ReadTime()
{
	uint8_t i = 0;

	Tools::I2C_Write(_addr, 0x00, I2C_SHORT_ADDR); 
	Wire.requestFrom(_addr, 7);

	if(Wire.available())
	{
		_rtc_sec = Tools::bcdToDec(Wire.read() & 0x7f);
		i++;
	}
	
	if(Wire.available())
	{
		_rtc_min = Tools::bcdToDec(Wire.read());
		i++;
	}
	
	if(Wire.available())
	{
		_rtc_hour = Tools::bcdToDec(Wire.read() & 0x3f);
		i++;
	}
	
	if(Wire.available())
	{
		_rtc_wday = Tools::bcdToDec(Wire.read());
		i++;
	}
	
	if(Wire.available())
	{
		_rtc_day = Tools::bcdToDec(Wire.read());
		i++;
	}
	
	if(Wire.available())
	{
		_rtc_mon = Tools::bcdToDec(Wire.read());
		i++;
	}
	
	if(Wire.available())
	{
		_rtc_year = Tools::bcdToDec(Wire.read());
		i++;
	}

	return i == 7;
}

void DS1307::WriteTime()
{
	uint8_t a[] = {_rtc_sec, _rtc_min, _rtc_hour,
					_rtc_wday, _rtc_day, _rtc_mon, _rtc_year};
	WriteTimeArray(a);
}

void DS1307::WriteTimeArray(uint8_t Array[])
{
	Wire.beginTransmission(_addr);
	Wire.write((uint8_t)0x00);
	for(int i = 0; i < 7; i++)
		Wire.write(Array[i]);
	Wire.endTransmission();
}



// Private Methods /////////////////////////////////////////////////////////////



// Preinstantiate Objects //////////////////////////////////////////////////////

DS1307 DS1307x = DS1307();

