/**
 * Arduino - Camera interface
 * 
 * Camera.h
 * 
 * The abstract class for a Camera.
 * 
 * @author Dalmir da Silva <dalmirdasilva@gmail.com>
 */

#ifndef __ARDUINO_DRIVER_CAMERA_H__
#define __ARDUINO_DRIVER_CAMERA_H__ 1

class Camera {
public:

    /**
     * Returns a frame.
     * 
     * @retun               A frame.
     */
    int readFrame(unsigned char *buf) = 0;
};

#endif /* __ARDUINO_DRIVER_CAMERA_H__ */
