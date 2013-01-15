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

// ***************************************************************************
// Bitmap routines
// ***************************************************************************

void DumpByte(byte Value)
{
  if( IS_B64_ENCODED )
    B64EncodeNext(Value);
  else
    Serial.write(Value);
}

void DumpString(const char *Value)
{
  byte b;
  while( (b = *Value++) )
    DumpByte(b);
}

void DumpInt(unsigned int Value)
{
  DumpByte(Value & 0x00FF);
  DumpByte((Value & 0xFF00) >> 8);
}

void DumpLong(unsigned long Value)
{
  DumpInt(Value & 0x0000FFFF);
  DumpInt((Value & 0xFFFF0000) >> 16);
}

void DumpBitmapHeader(boolean BW)
{
  unsigned long lng;
  
  // BitmapFileHeader
  DumpString("BM");     // bfType
  lng = (BW ? 77878 : 230454);
  DumpLong(lng);        // bfSize
  DumpLong(0);          // bfReserved1
  lng = (BW ? 1078 : 54);
  DumpLong(lng);        // bfOffbits

  // BitmapInfoHeader
  DumpLong(40);         // biSize
  DumpLong(320);        // biWidth
  DumpLong(240);        // biHeight
  DumpInt(1);           // biPlanes
  lng = (BW ? 8 : 24);
  DumpInt((int)lng);    // biBitCount
  DumpLong(0);          // biCompression
  lng = (BW ? 76800 : 230400);
  DumpLong(lng);        // biSizeImage
  DumpLong(0);          // biXPPerMeter
  DumpLong(0);          // biYPPerMeter
  lng = (BW ? 256 : 0);
  DumpLong(lng);        // biClrUsed
  DumpLong(0);          // biClrImportant
  
  if(BW)
  {
    // create b/w palette
    for(int i = 0; i < 256; i++)
    {
      DumpByte(i);
      DumpByte(i);
      DumpByte(i);
      DumpByte(0);
    }
  }  
}

void DumpUYV(byte U, byte Y, byte V)
{
  byte R;
  byte G;
  byte B;

  Cam.UYV2RGB(U, Y, V, &R, &G, &B);
  DumpRGB(R, G, B);
}

void DumpRGB(byte R, byte G, byte B)
{
  DumpByte(R);
  DumpByte(G);
  DumpByte(B);
}


