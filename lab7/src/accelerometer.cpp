#include <accelerometer.h>

Accelerometer::Accelerometer(){}

void Accelerometer::begin(){
    if (!acc.begin()) {
        Serial.println("Failed to initialize the Accelerometer");
        while (1) {
          delay(10);
        }
    }
    acc.setAccelerometerRange(MPU6050_RANGE_16_G);
    acc.setGyroRange(MPU6050_RANGE_2000_DEG);
    acc.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void Accelerometer::getEvent(sensors_event_t *event){
    sensors_event_t g, t;
    acc.getEvent(event, &g, &t);
}

Orientation Accelerometer::getOrientation(){
    sensors_event_t event;
    getEvent(&event);
    float x = event.acceleration.x;
    float y = event.acceleration.y;
    float z = event.acceleration.z;
    float pitch = atan2(x, sqrt(y * y + z * z)) * 180 / M_PI;
    float roll = atan2(y, sqrt(x * x + z * z)) * 180 / M_PI;
    float yaw = atan2(z, sqrt(x * x + y * y)) * 180 / M_PI;
    return {pitch, roll, yaw};
}