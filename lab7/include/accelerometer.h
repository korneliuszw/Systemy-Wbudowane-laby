#ifndef _ACCELEROMETER_H
#define _ACCELEROMETER_H

#include <Adafruit_MPU6050.h>
// #include <Adafruit_ADXL345_U.h>

#include <orientation.h>

class Accelerometer {
    Adafruit_MPU6050 acc;
    public:
        Accelerometer();
        void begin();
    
    void getEvent(sensors_event_t *event);

    Orientation getOrientation();
};

#endif // _ACCELEROMETER_H