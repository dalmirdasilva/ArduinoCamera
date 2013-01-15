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
// TC74 I2C Temp sensor
// ***************************************************************************

void PrintTemp()
{
  StartTextResult(false);

  char temp = TC74x.ReadTemperature();
  if((unsigned byte)temp == TC74_VALUE_ERROR)
      SendERR(NULL);
  else
  {
    Serial.print(temp, DEC);
    Serial.println();
    SendOK(NULL);
  }
}

