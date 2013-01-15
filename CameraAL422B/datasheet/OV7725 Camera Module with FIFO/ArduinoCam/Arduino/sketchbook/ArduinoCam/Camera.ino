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

#include <Arduino.h>
#include <Wire.h>
#include <MIN_at_Tools.h>
#include <MIN_at_Camera.h>
#include <MIN_at_TC74.h>
#include <MIN_at_DS1307.h>

#define FLAG_SHOW_PROMPT       1
#define SHOW_PROMPT            ((prgFlags & FLAG_SHOW_PROMPT) == FLAG_SHOW_PROMPT)

#define FLAG_B64_OUTPUT        2
#define IS_B64_ENCODED         ((prgFlags & FLAG_B64_OUTPUT) == FLAG_B64_OUTPUT)

byte prgFlags = FLAG_B64_OUTPUT | FLAG_SHOW_PROMPT;
TC74 TC74x = TC74();

void setup()
{
  Serial.begin(115200);
  Wire.begin();

  Cam.Begin();
  TC74x.Begin(TC74A5_ADDR);
  DS1307x.Begin();

  InitCam();  
}

void loop()
{
  byte inByte;
  boolean result;

  if(SHOW_PROMPT)
    Serial.print(">");

  if(Serial.available() > 0) 
  {
    inByte = Serial.read();
    Serial.write(inByte);
    Serial.println();

    switch(inByte)
    {
    case 'i':  // initialize camera
      StartTextResult(false);
      InitCam();
      SendOK(NULL);
      break;
    case 's':  // show camera register config
      StartTextResult(true);
      Cam.DumpConfig();
      SendOK(NULL);
      break;
    case 'x':  // take picture
      StartTextResult(false);
      if(!Cam.Capture())
        SendERR(NULL);
      else
        SendOK(NULL);
      break;
      
    case 'T':  // color testbar on
      StartTextResult(false);
      Cam.ColorBar(true);
      SendOK(NULL);
      break;
    case 't':  // color testbar off
      StartTextResult(false);
      Cam.ColorBar(false);
      SendOK(NULL);
      break;
      
    case 'M':
      StartTextResult(false);
      Cam.Mirror(true);
      SendOK(NULL);
      break;
    case 'm':
      StartTextResult(false);
      Cam.Mirror(false);
      SendOK(NULL);
      break;
      
    case 'd':  // hex dump picture
      StartTextResult(true);
      Cam.Dump(true);
      SendOK(NULL);
      break;
    case 'D':  // binary dump picture
      StartBinaryResult();
      Cam.Dump(false);
      SendOK(NULL);
      break;


    case 'E':  // encode B64 on
      StartTextResult(false);
      prgFlags |= FLAG_B64_OUTPUT;
      SendOK(NULL);
      break;
    case 'e':  // encode B64 off
      StartTextResult(false);
      prgFlags &= ~(FLAG_B64_OUTPUT);
      SendOK(NULL);
      break;


    case 'R':  // dump raw picture data in color
      StartPictureResult();
      ( DumpRaw(false) ? SendOK(NULL) : SendERR(NULL) );
      break;
    case 'r':  // dump raw picture data in BW
      StartPictureResult();
      ( DumpRaw(true) ? SendOK(NULL) : SendERR(NULL) );
      break;
      
    case 'B':  // dump picture as bitmap in color
      StartPictureResult();
      ( DumpPictureAsBitmap(false) ? SendOK(NULL) : SendERR(NULL) );
      break;
    case 'b':  // dump picture as bitmap in BW
      StartPictureResult();
      ( DumpPictureAsBitmap(true) ? SendOK(NULL) : SendERR(NULL) );
      break;


    case 'c':
      PrintTemp();
      break;

    case 'I':
      SetTime();
      break;
    case 'C':
      PrintTime();
      break;

    default:
      StartTextResult(false);
      SendOK("UNK");
    }
    prgFlags |= FLAG_SHOW_PROMPT;
  }
  else
  {
    prgFlags &= ~(FLAG_SHOW_PROMPT);
    delay(200);
  }
}

void InitCam()
{
  if(Cam.Reset())
    Cam.Init();
}

boolean DumpRaw(boolean BW)
{
  byte b;

  if( IS_B64_ENCODED )
    B64EncodeStart("--- B64 start ---\n");

  Cam.ResetVideoPointer();

  // 76800 = 320 * 240	
  for(unsigned long lc = 0; lc < 76800; lc++)
  {
    if(IsCanceled())
    {
      Serial.println();
      return false;
    }

    // read U/V
    b = Cam.ReadNextVideoByte();
    if(!BW)
      DumpByte(b);

    // read Y
    b = Cam.ReadNextVideoByte();
    DumpByte(b);
  }

  digitalWrite(BUS_RCK, LOW);

  if( IS_B64_ENCODED )
    B64EncodeDone("\n--- B64 end ---");
  Serial.println();

  return true;
}

boolean DumpPictureAsBitmap(boolean BW)
{
  if( IS_B64_ENCODED )
    B64EncodeStart("--- B64 start ---\n");

  DumpBitmapHeader(BW);

  Cam.ResetVideoPointer();

  // UYVY Images are stored top-down, so the upper left pixel starts at byte 0.
  // Each 4 bytes represent the color for 2 neighboring pixels:
  // 
  // [ U0 | Y0 | V0 | Y1 ]
  //
  // Y0 is the brightness of pixel 0, Y1 the brightness of pixel 1.
  // U0 and V0 is the color of both pixels.

  byte U;
  byte Y;
  byte V;

  for(int lc = 0; lc < 240; lc++)
  {
    for(int pc = 0; pc < 160; pc++)
    {
      if(IsCanceled())
      {
        Serial.println();
        return false;
      }

      U = Cam.ReadNextVideoByte();
      Y = Cam.ReadNextVideoByte();
      V = Cam.ReadNextVideoByte();

      if(BW)
      {
        DumpByte(Y);
        Y = Cam.ReadNextVideoByte();
        DumpByte(Y);
      }
      else
      {
        DumpUYV(U, Y, V);
        Y = Cam.ReadNextVideoByte();
        DumpUYV(U, Y, V);
      }
    }
  }

  digitalWrite(BUS_RCK, LOW);

  if( IS_B64_ENCODED )
    B64EncodeDone("\n--- B64 end ---");
  Serial.println();

  return true;
}



void StartTextResult(boolean MultiLine)
{
  Serial.print( (MultiLine ? "=\r\n" : "-\r\n") );
}

void StartBinaryResult()
{
  Serial.print("@\r\n");
}

void StartPictureResult()
{
  if( IS_B64_ENCODED )
    StartTextResult(true);
  else
    StartBinaryResult();
}

void SendOK(char *Message)
{
  Serial.print("OK ");
  SendMSG(Message);
  Serial.println();
}

void SendERR(char *Message)
{
  Serial.print("ERR ");
  SendMSG(Message);
  Serial.println();
}

void SendMSG(char *Message)
{
  if(Message != NULL)
    Serial.print(Message);
}

boolean IsCanceled()
{
  byte inByte;

  if(Serial.available() > 0) 
  {
    inByte = Serial.read();
    if(inByte == '.')
      return true;
  }

  return false;
}


