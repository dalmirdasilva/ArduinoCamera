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
// Base 64 routines
// ***************************************************************************

inline void a3_to_a4(unsigned char *a4, unsigned char *a3);

const char b64_alphabet[] = 
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz"
                "01234567890/+";
unsigned long _byteCount;
byte _byteMod;
byte _outCount;
unsigned char a3[3];
unsigned char a4[4];

void B64EncodeStart(char *Header)
{
  _byteCount = 0;
  _byteMod = 0;
  _outCount = 0;
  B64ClearMem();
  
  if(Header != NULL)
    Serial.print(Header);
}

void B64EncodeNext(unsigned char value)
{
  if(_byteMod == 0 && _byteCount > 0)
  {
    a3_to_a4(a4, a3);
    for(int i = 0; i < 4; i++)
      B64Out(b64_alphabet[a4[i]]);    
    B64ClearMem();
  }
  
  a3[_byteMod] = value;
  
  _byteCount++;
  _byteMod = _byteCount % 3;
}

void B64EncodeDone(char *Header)
{
  if(_byteMod == 0)
    if(_byteCount == 0)
      return;
    else
      _byteMod = 3;
    
  a3_to_a4(a4, a3);

  for(int i = 0; i < 4; i++)
    B64Out( (i <= _byteMod ? b64_alphabet[a4[i]] : '=') );
    
  if(Header != NULL)
    Serial.print(Header);
}

void B64Out(char Value)
{
  Serial.write(Value);
  _outCount++;
  if(_outCount == 76)
  {
    Serial.println();
    _outCount = 0;
  }
}

void B64ClearMem()
{
  a3[0] = 0;
  a3[1] = 0;
  a3[2] = 0;
}

inline void a3_to_a4(unsigned char *a4, unsigned char *a3)
{
  a4[0] = (a3[0] & 0xfc) >> 2;
  a4[1] = ((a3[0] & 0x03) << 4) + ((a3[1] & 0xf0) >> 4);
  a4[2] = ((a3[1] & 0x0f) << 2) + ((a3[2] & 0xc0) >> 6);
  a4[3] = (a3[2] & 0x3f);
}





