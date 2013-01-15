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

#include "MIN_at_Tools.h"



// Constructors ////////////////////////////////////////////////////////////////

Tools::Tools()
{
}



// Public Methods //////////////////////////////////////////////////////////////

int Tools::ReadDec(uint8_t MaxLen, uint8_t Flags, bool *Valid)
{
	int v = 0;
	uint8_t i = 0;
	uint8_t inByte;

	*Valid = true;

	while(true)
	{
		if(Serial.available() > 0) 
		{
			inByte = Serial.read();

			// ESC = cancel
			if(inByte == 0x1B)
			{
				*Valid = false;
				break;
			}

			// handle backspace
			if(inByte == CHAR_BACKSPACE)
			{
				if(i <= 0)
					Serial.write(CHAR_BELL);
				else
				{
					v = v / 10;
					i--;
					if( (Flags & DI_ECHO) == DI_ECHO )
						Serial.print("\010 \010");
				}
				continue;
			}

			// CR & LF submits input
			if( (inByte == CHAR_CR || inByte == CHAR_LF) && (Flags & DI_ALLOW_CR) == DI_ALLOW_CR )
				break;

			// if maximum allowed input length reached
			if(i >= MaxLen)
			{
				Serial.write(CHAR_BELL);
				continue;
			}

			// only digits 0 - 9 allowed
			if(inByte >= '0' && inByte <= '9')  
			{
				if( (Flags & DI_ECHO) == DI_ECHO )
					Serial.write(inByte);

				v = (v * 10) + (inByte - '0');
				i++;
				if(i >= MaxLen && (Flags & DI_AUTO_SKIP) == DI_AUTO_SKIP)
					break;
			}
			else
				Serial.write(CHAR_BELL);
		}
	}

	return v;
}

char *Tools::FormatHEX(uint8_t Value, uint8_t Prefix)
{
	static char strOut[5];
	snprintf(strOut, sizeof(strOut), "%s%02X", (Prefix > 0 ? "0x" : ""), Value);
	return strOut;
}

char *Tools::FormatHEX16(int Value, uint8_t Prefix)
{
	static char strOut[7];
	snprintf(strOut, sizeof(strOut), "%s%04X", (Prefix > 0 ? "0x" : ""), Value);
	return strOut;
}

char *Tools::FormatBIN(uint8_t Value)
{
	static char buffer[9];
	for (int i = 0; i < 8; i++) 
		buffer[7-i] = '0' + ((Value & (1 << i)) > 0);
	buffer[8] ='\0';
	return buffer;
}





void Tools::I2C_Write(uint8_t I2cAddr, uint16_t MemAddr, uint8_t UseLongAddr)
{
	Wire.beginTransmission(I2cAddr);
	if(UseLongAddr > 0)
	{
		Wire.write(MemAddr >> 8); // MSB
		Wire.write(MemAddr & 0xFF); // LSB
	}
	else
		Wire.write(MemAddr & 0x00FF);
	Wire.endTransmission();
}

void Tools::I2C_WriteValue(uint8_t I2cAddr, uint16_t MemAddr, uint8_t UseLongAddr,
							uint8_t Value, int Delay)
{
	Wire.beginTransmission(I2cAddr);
	if(UseLongAddr > 0)
	{
		Wire.write(MemAddr >> 8); // MSB
		Wire.write(MemAddr & 0xFF); // LSB
	}
	else
		Wire.write(MemAddr & 0x00FF);
	Wire.write(Value);
	Wire.endTransmission();
	if(Delay > 0)
		delay(Delay);
}

bool Tools::I2C_ReadByte(uint8_t I2cAddr, uint16_t MemAddr, uint8_t UseLongAddr,
							uint8_t *Value)
{
	I2C_Write(I2cAddr, MemAddr, UseLongAddr);
	Wire.requestFrom((int)I2cAddr, 1);

	if(Wire.available())
	{
		*Value = Wire.read();
		return true;
	}
	else
		return false;
}

bool Tools::I2C_ReadByteDefault(uint8_t I2cAddr, uint16_t MemAddr, uint8_t UseLongAddr,
								uint8_t *Value, uint8_t DefaultValue)
{
	if(!Tools::I2C_ReadByte(I2cAddr, MemAddr, UseLongAddr, Value))
	{
		*Value = DefaultValue;
		return true;
	}
	
	return false;
}

void Tools::I2C_SetBitAt(uint8_t I2cAddr, uint16_t MemAddr, uint8_t UseLongAddr,
							uint8_t BitNum, bool Value, int Delay)
{
	uint8_t m = 1 << BitNum;
	uint8_t b;
	if(!I2C_ReadByte(I2cAddr, MemAddr, UseLongAddr, &b))
		return;
	
	if(!Value)
	{
		m = ~m;
		b = b & m;
	}
	else
		b = b | m;
		
	I2C_WriteValue(I2cAddr, MemAddr, UseLongAddr, b, Delay);
}

// WARNING: address is a page address, 6-bit end will wrap around
// also, data can be maximum of about 30 bytes, because the Wire 
// library has a buffer of 32 bytes
void Tools::I2C_EEWriteBuffer(uint8_t I2cAddr, uint16_t MemAddr, 
						byte *Data, byte Length) 
{
  Wire.beginTransmission(I2cAddr);
  Wire.write(MemAddr >> 8); // MSB
  Wire.write(MemAddr & 0x00FF); // LSB
  byte c;
  for (c = 0; c < Length; c++)
    Wire.write(Data[c]);
  Wire.endTransmission();
}

// maybe let's not read more than 30 or 32 bytes at a time!
void Tools::I2C_EEReadBuffer(uint8_t I2cAddr, uint16_t MemAddr, byte *Data, int Length)
{
  Wire.beginTransmission(I2cAddr);
  Wire.write(MemAddr >> 8); // MSB
  Wire.write(MemAddr & 0xFF); // LSB
  Wire.endTransmission();
  Wire.requestFrom((int)I2cAddr, Length);
  int c = 0;
  for (c = 0; c < Length; c++)
    if (Wire.available()) 
		Data[c] = Wire.read();
}

// Convert binary coded decimal to normal decimal numbers
uint8_t Tools::bcdToDec(uint8_t Value)
{
	return ( (Value / 16 * 10) + (Value % 16) );
}

// Convert Decimal to Binary Coded Decimal (BCD)
uint8_t Tools::dec2bcd(uint8_t num)
{
	return ( (num / 10 * 16) + (num % 10) );
}



// Private Methods //////////////////////////////////////////////////////////////



// Preinstantiate Objects //////////////////////////////////////////////////////

