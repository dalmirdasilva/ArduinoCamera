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

#include "MIN_at_Camera.h"



// OV7620 Arduino camera shield 



// Constructors ////////////////////////////////////////////////////////////////

Camera::Camera()
{
}



// Public Methods //////////////////////////////////////////////////////////////

void Camera::Begin()
{
	Begin(OV_7620_ADDR);
}

void Camera::Begin(byte address)
{
	Begin((int)address);
}

void Camera::Begin(int address)
{
	_addr = address;
  
	DDRD = B00000000;
	DDRC = B00000000;
	
	pinModeFast(BUS_RRST, OUTPUT);
	pinModeFast(BUS_RCK, OUTPUT);
  
	digitalWriteFast(BUS_RRST, HIGH);
	digitalWriteFast(BUS_RCK, LOW);
}

bool Camera::Reset()
{
	byte b = 0;

	// (0x12) initiate camera reset
	Tools::I2C_WriteValue(_addr, OV_COMMON_A, I2C_SHORT_ADDR, 0x80, 300);

	// try to read manufacturer id 0x7FA2 from camera
	Tools::I2C_Write(_addr, OV_ID_H, I2C_SHORT_ADDR);  
	Wire.requestFrom(_addr, 2);

	if(Wire.available())
	{
		b = Wire.read();
		if(b != 0x7F)
			return false;
	}
	else
		return false;

	if(Wire.available())
	{
		b = Wire.read();
		if(b != 0xA2)
			return false;
	}
	else
		return false;

	return true;
}

void Camera::Init()
{
	ResetVideoPointer();
	
	// (0x11) HSYNC-neg, CHSYNC-neg, VSYNC-pos
	//Tools::I2C_WriteValue(_addr, OV_CLOCK_RATE, I2C_SHORT_ADDR, 0x00, 10);
  
	// (0x12) Mirror image, auto white balance, AGC
	//Tools::I2C_WriteValue(_addr, OV_COMMON_A, I2C_SHORT_ADDR, 0x64, 10);
	
	// (0x13) set 8bit mode, enable bus, auto adjust
	Tools::I2C_WriteValue(_addr, OV_COMMON_B, I2C_SHORT_ADDR, 0x21, 10);
  
  
	// (0x14) set 640 x 480 VGA mode
	Tools::I2C_WriteValue(_addr, OV_COMMON_C, I2C_SHORT_ADDR, 0x04, 10);
	// (0x14) set 320 x 240 QVGA mode
	//Tools::I2C_WriteValue(_addr, OV_COMMON_C, I2C_SHORT_ADDR, 0x24, 10);
  
  
	// (0x15) set UYVY for 8 bit output
	Tools::I2C_WriteValue(_addr, OV_COMMON_D, I2C_SHORT_ADDR, 0x01, 10);
  
	// (0x16) field mode selection off
	Tools::I2C_WriteValue(_addr, OV_FRAME_DROP, I2C_SHORT_ADDR, 0x00, 10);
  
	// (0x27) set SRAM control
	Tools::I2C_WriteValue(_addr, OV_COMMON_G, I2C_SHORT_ADDR, 0xE3, 10);
  
  
	// (0x28) set interlaced scan mode
	Tools::I2C_WriteValue(_addr, OV_COMMON_H, I2C_SHORT_ADDR, 0x00, 10);
	// (0x28) set progressive scan mode
	//Tools::I2C_WriteValue(_addr, OV_COMMON_H, I2C_SHORT_ADDR, 0x20, 10);
	
	
	// (0x67) set color space
	// 0x1A = YUV, 0x5A = Analog YUV, 0x9A = CCIR 601 YCrCb
	// 0xDA = PAL YUV
	//Tools::I2C_WriteValue(_addr, OV_COLOR_SPACE, I2C_SHORT_ADDR, 0x1A, 10);
}

void Camera::ColorBar(bool Value)
{
	Tools::I2C_SetBitAt(_addr, OV_COMMON_A, I2C_SHORT_ADDR, 1, Value, 0);
}

void Camera::Power(bool Value)
{
	Tools::I2C_SetBitAt(_addr, OV_COMMON_O, I2C_SHORT_ADDR, 5, Value, 0);
}

void Camera::Mirror(bool Value)
{
	Tools::I2C_SetBitAt(_addr, OV_COMMON_A, I2C_SHORT_ADDR, 6, Value, 0);
}

bool Camera::Capture()
{
	byte b = 0;
	bool status = false;
	int i = 0;

	// (0x14) set 320 x 240 QVGA mode
	Tools::I2C_WriteValue(_addr, OV_COMMON_C, I2C_SHORT_ADDR, 0x24, 10);
	
	// (0x28) set progressive scan mode
	Tools::I2C_WriteValue(_addr, OV_COMMON_H, I2C_SHORT_ADDR, 0x20, 10);

	// start single frame transfer
	Tools::I2C_WriteValue(_addr, OV_COMMON_B, I2C_SHORT_ADDR, 0x23, 100);
	
again:
	i++;
    delay(5);
    
	b = Cam.ReadConfigByte(OV_COMMON_B);
	if( (b & 0x02) == 0x00 )
	{
		status = true;
		i = 11;
	}

	if(i < 10)
		goto again;
  
  
	// (0x14) set 640 x 480 VGA mode
	Tools::I2C_WriteValue(_addr, OV_COMMON_C, I2C_SHORT_ADDR, 0x04, 10);
	
	// (0x28) set interlaced scan mode
	Tools::I2C_WriteValue(_addr, OV_COMMON_H, I2C_SHORT_ADDR, 0x00, 10);

	return status;
}

void Camera::Dump(bool Hex)
{
	byte cntr = 0;
	byte b = 0;
  
	ResetVideoPointer();
	
	for(int lc = 0; lc < 240; lc++)
	{
		for(int pc = 0; pc < 640; pc++)
		{
			// read U/V, Y
			b = ReadNextVideoByte();
			
			if(Hex)
				DumpVideoByte(b, &(++cntr));
			else
				Serial.write(b);
		}
	}
  
	digitalWriteFast(BUS_RCK, LOW);
	Serial.println();
}

void Camera::DumpConfig()
{
	byte b = 0;

	// (0x11) HSYNC-neg, CHSYNC-neg, VSYNC-pos
	Serial.print("OV_CLOCK_RATE (0x11: 0x00): ");
	b = Cam.ReadConfigByte(OV_CLOCK_RATE);
	Camera::DebugPrintValue(b);

	// (0x12) mirror image, testpattern, reset
	Serial.print("OV_COMMON_A   (0x12: 0x64): ");
	b = Cam.ReadConfigByte(OV_COMMON_A);
	Camera::DebugPrintValue(b);

	// (0x13) set 8bit mode, enable bus, auto adjust
	Serial.print("OV_COMMON_B   (0x13: 0x21): ");
	b = Cam.ReadConfigByte(OV_COMMON_B);
	Camera::DebugPrintValue(b);

	// (0x14) set 320 x 240 QVGA mode
	Serial.print("OV_COMMON_C   (0x14: 0x24): ");
	b = Cam.ReadConfigByte(OV_COMMON_C);
	Camera::DebugPrintValue(b);

	// (0x16) field mode selection off
	Serial.print("OV_FRAME_DROP (0x16: 0x00): ");
	b = Cam.ReadConfigByte(OV_FRAME_DROP);
	Camera::DebugPrintValue(b);

	// (0x27) set SRAM control
	Serial.print("OV_COMMON_G   (0x27: 0xE3): ");
	b = Cam.ReadConfigByte(OV_COMMON_G);
	Camera::DebugPrintValue(b);

	// (0x28) set progressive scan mode
	Serial.print("OV_COMMON_H   (0x28: 0x20): ");
	b = Cam.ReadConfigByte(OV_COMMON_H);
	Camera::DebugPrintValue(b);

	// (0x67) color space selection
	Serial.print("OV_COLOR_SPACE(0x67: 0x1A): ");
	b = Cam.ReadConfigByte(OV_COLOR_SPACE);
	Camera::DebugPrintValue(b);
 
	// (0x71) set progressive scan mode
	Serial.print("OV_COMMON_L   (0x71: 0x00): ");
	b = Cam.ReadConfigByte(OV_COMMON_L);
	Camera::DebugPrintValue(b);
}

byte Camera::ReadConfigByte(byte MemAddr)
{
	byte b = 0;

	Tools::I2C_ReadByteDefault(_addr, MemAddr, I2C_SHORT_ADDR, &b, 0);
	
	return b;
}

uint8_t Camera::ReadNextVideoByte()
{
	// read clock

	digitalWriteFast(BUS_RCK, LOW);
	delayMicroseconds(20);

	digitalWriteFast(BUS_RCK, HIGH);
	delayMicroseconds(20);
	
	return (PIND & B11111100) | (PINC & B00000011);
}

void Camera::DumpVideoByte(byte VideoByte, byte *count)
{
	Serial.print(Tools::FormatHEX(VideoByte, 0));
  
	if(*count == 16)
	{
		*count = 0;
		Serial.println();
	}
	else
		Serial.print(" ");
}

void Camera::ResetVideoPointer()
{
	// reset sram read/write pointer
	digitalWriteFast(BUS_RRST, LOW);
	delay(1);
	
	// read clock
	digitalWriteFast(BUS_RCK, LOW);
	delay(1);
	digitalWriteFast(BUS_RCK, HIGH);
	delay(1);
	digitalWriteFast(BUS_RCK, LOW);
	delay(1);
	digitalWriteFast(BUS_RCK, HIGH);
	delay(1);
	
	digitalWriteFast(BUS_RRST, HIGH);
}

void Camera::DebugPrintValue(byte Value)
{
  Serial.print(Tools::FormatHEX(Value, 1));
  Serial.print(" ");
  Serial.print(Tools::FormatBIN(Value));
  Serial.println();
}

void Camera::UYV2RGB(byte U, byte Y, byte V, byte *R, byte *G, byte *B)
{
  float Yx = 1.164 * (Y - 16.0);
  float Ux = U - 128.0;
  float Vx = V - 128.0;
  
  *R = Clip(Yx + 1.596 * Ux);
  *G = Clip(Yx - 0.813 * Ux - 0.392 * Vx);
  *B = Clip(Yx + 2.017 * Vx);
}



// Private Methods //////////////////////////////////////////////////////////////

uint8_t Camera::Clip(float Value)
{
  float v = round(Value);
  
  if(v < 0) 
    return 0;
    
  if(v > 255) 
    return 255;
    
  return (byte)Value;
}



// Preinstantiate Objects //////////////////////////////////////////////////////

Camera Cam = Camera();

