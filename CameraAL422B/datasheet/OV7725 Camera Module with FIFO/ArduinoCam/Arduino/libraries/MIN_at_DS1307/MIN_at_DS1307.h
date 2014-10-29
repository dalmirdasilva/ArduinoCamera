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

#ifndef MIN_at_DS1307_h
#define MIN_at_DS1307_h

#include <inttypes.h>
#include <Arduino.h>
#include "MIN_at_Tools.h"


#define DS1307_ADDR				0x68

#define DS1307_SECONDS			0x00
#define DS1307_MINUTES			0x01
#define DS1307_HOURS			0x02
#define DS1307_DAY				0x03
#define DS1307_DATE				0x04
#define DS1307_MONTH			0x05
#define DS1307_YEAR				0x06
#define DS1307_CONFIG			0x07

class DS1307
{
  private:
	int _addr;
	
  
  public:
	uint8_t _rtc_sec;
	uint8_t _rtc_min;
	uint8_t _rtc_hour;
	uint8_t _rtc_wday;
	uint8_t _rtc_day;
	uint8_t _rtc_mon;
	uint8_t _rtc_year;
  
    DS1307();
	
	void Begin();
	void Begin(uint8_t address);
	void Begin(int address);
	
	void Reset();
	uint8_t ReadConfigByte();
	void WriteConfigByte(uint8_t value);	
	bool ReadTime();
	void WriteTime();
	void WriteTimeArray(uint8_t Array[]);
};

extern DS1307 DS1307x;

#endif
