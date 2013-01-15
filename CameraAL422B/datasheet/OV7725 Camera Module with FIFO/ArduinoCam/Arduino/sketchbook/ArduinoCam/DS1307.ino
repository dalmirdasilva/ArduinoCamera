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
// DS1307 I2C RTC
// ***************************************************************************

void SetTime()
{
  Serial.print(" YY.MM.DD WD hh:mm:ss\r\n");
  Serial.print(":");
  
  bool valid;
  byte a[7];
  byte b;
  for(char i = 6; i >= 0; i--)
  {
    b = Tools::ReadDec(2, DI_ECHO | DI_AUTO_SKIP, &valid);
    a[i] = Tools::dec2bcd(b);
    if(!valid)
      break;
    if(i > 0)
      Serial.print(i == 4 || i == 3 ? " " : (i < 3 ? ":" : "."));
  }
  
  Serial.println();
  if(valid)
  {
    //for(char i = 6; i >= 0; i--)
    //  Serial.println(a[i], HEX);
    DS1307x.WriteTimeArray(a);
    SendOK(NULL);
  }
  else
    SendERR(NULL);
}

void PrintTime()
{
  StartTextResult(false);

  if(DS1307x.ReadTime())
  {
    PrintLeading(DS1307x._rtc_hour);
    Serial.print(":");
    PrintLeading(DS1307x._rtc_min);
    Serial.print(":");
    PrintLeading(DS1307x._rtc_sec);
    Serial.print("\r\n");
    
    SendOK(NULL);
  }
  else
    SendERR(NULL);
}

void PrintLeading(byte Value)
{
  if(Value < 10)
    Serial.print("0");
  Serial.print(Value, DEC);
}


