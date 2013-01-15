/**
 * Arduino - CameraAL422B implementation.
 * 
 * CameraAL422B.cpp
 * 
 * The class CameraAL422B.
 * 
 * @author Dalmir da Silva <dalmirdasilva@gmail.com>
 */

#ifndef __ARDUINO_DRIVER_CAMERA_AL422B_CPP__
#define __ARDUINO_DRIVER_CAMERA_AL422B_CPP__ 1

CameraAL422B::CameraAL422B(unsigned char (*read)(), unsigned char vsyncPin, unsigned char hsyncPin, unsigned char pclkPin) : Camera() {
    this->read = read;
    this->vsyncPin = vsyncPin;
    this->hsyncPin = hsyncPin;
    this->pclkPin = pclkPin;
    address = 0x42;
    Wire.begin();
}

int CameraAL422B::readFrame(OutputStream *out) {
    
}

#endif /* __ARDUINO_DRIVER_CAMERA_AL422B_CPP__ */
