/**
 * Arduino - CameraOV7670 implementation.
 * 
 * CameraOV7670.cpp
 * 
 * The class CameraOV7670.
 * 
 * @author Dalmir da Silva <dalmirdasilva@gmail.com>
 */

#ifndef __ARDUINO_DRIVER_CAMERA_OV7670_CPP__
#define __ARDUINO_DRIVER_CAMERA_OV7670_CPP__ 1

#include "CameraOV7670.h"

CameraOV7670::CameraOV7670(unsigned char (*read)(), unsigned char vsyncPin,
        unsigned char hsyncPin) :
        Camera() {
    this->read = read;
    this->vsyncPin = vsyncPin;
    this->hsyncPin = hsyncPin;
    address = 0x42;
    Wire.begin();
}

int CameraOV7670::readFrame(OutputStream *out) {
    return 0;
}

#endif /* __ARDUINO_DRIVER_CAMERA_OV7670_CPP__ */
