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

#include "CameraAL422B.h"

CameraAL422B::CameraAL422B(unsigned char (*read)(), unsigned char vsyncPin,
        unsigned char writeEnPin, unsigned char readClockPin,
        unsigned char readResetPin) :
        Camera(), read(read) {
    this->vsyncPin = vsyncPin;
    this->writeEnPin = writeEnPin;
    this->readClockPin = readClockPin;
    this->readResetPin = readResetPin;
    address = 0x42;
    width = 640;
    height = 480;
}

void CameraAL422B::begin() {
    Wire.begin();
    pinMode(vsyncPin, INPUT);
    pinMode(writeEnPin, OUTPUT);
    pinMode(readClockPin, OUTPUT);
    pinMode(readResetPin, OUTPUT);
    resetRegisters();
    disableWrite();
    delayMicroseconds(100);
}

bool CameraAL422B::capture() {
    while (digitalReadFast(vsyncPin));
    while (!digitalReadFast(vsyncPin));
    enableWrite();
    while (digitalReadFast(vsyncPin));
    disableWrite();
    return true;
}

int CameraAL422B::readFrame(OutputStream *out) {
    int i, n = 0;
    resetReadPointer();
    for (i = 0; i < height; i++) {
        n += readRow(out);
    }
    return n;
}

int CameraAL422B::readRow(OutputStream *out) {
    int i;
    for (i = 0; i < width; i++) {
        digitalWriteHighFast(readClockPin);
        out->write(read());
        digitalWriteLowFast(readClockPin);
    }
    return i;
}

void CameraAL422B::setHorizontalMirror(bool mirror) {
    configureRegisterBits(MVFP, MVFP_MIRROR, (mirror) ? MVFP_MIRROR : 0x00);
}

void CameraAL422B::setVerticalFlip(bool flip) {
    configureRegisterBits(MVFP, MVFP_FLIP, (flip) ? MVFP_FLIP : 0x00);
}

void CameraAL422B::setFlashlightModeSelect(FlashlightModeSelect mode) {
    configureRegisterBits(STR_OPT, STR_OPT_MODE, (unsigned char) mode);
}

void CameraAL422B::setStrobeRequest(bool request) {
    configureRegisterBits(STR_OPT, STR_OPT_REQUEST,
            (request) ? STR_OPT_REQUEST : 0x00);
}

void CameraAL422B::setColorGainControlEnable(bool enable) {
    configureRegisterBits(STR_OPT, STR_OPT_GAIN,
            (enable) ? STR_OPT_GAIN : 0x00);
}

void CameraAL422B::resetRegisters() {
    configureRegisterBits(COM7, COM7_RESET, 0xff);
}

void CameraAL422B::setOutputFormat(OutputFormat format) {
    configureRegisterBits(COM7, COM7_FORMAT, (unsigned char) format);
}

void CameraAL422B::setOutputResolution(OutputResolution resolution) {
    configureRegisterBits(COM7, COM7_RESOLUTION, (unsigned char) resolution);
    switch (resolution) {
        case VGA:
            width = 640;
            height = 480;
            break;
        case QVGA:
            width = 320;
            height = 240;
            break;
        case CIF:
            width = 352;
            height = 288;
            break;
        case QCIF:
            width = 176;
            height = 144;
            break;
    }
}

void CameraAL422B::setRGBOutput(RGBOutput output) {
    configureRegisterBits(COM15, COM15_RGB, (unsigned char) output);
}

void CameraAL422B::enableWrite() {
    digitalWriteLowFast(writeEnPin);
}

void CameraAL422B::disableWrite() {
    digitalWriteHighFast(writeEnPin);
}

void CameraAL422B::resetReadPointer() {
    digitalWriteLowFast(readResetPin);
    delayMicroseconds(100);
    digitalWriteHighFast(readResetPin);
}

void CameraAL422B::configureRegisterBits(Register reg, Mask mask,
        unsigned char v) {
    unsigned char n;
    n = readRegister(reg);
    n &= ~((unsigned char) mask);
    n |= v & ((unsigned char) mask);
    writeRegister(reg, n);
}

void CameraAL422B::writeRegister(Register reg, unsigned char v) {
    Wire.beginTransmission(address);
    Wire.write((unsigned char) reg);
    Wire.write(v);
    Wire.endTransmission();
}

unsigned char CameraAL422B::readRegister(Register reg) {
    Wire.beginTransmission(address);
    Wire.write((unsigned char) reg);
    Wire.endTransmission(false);
    Wire.requestFrom(address, (unsigned char) 1);
    while (!Wire.available()) {
        delay(10);
    }
    return Wire.read();
}

#endif /* __ARDUINO_DRIVER_CAMERA_AL422B_CPP__ */
