#include "ph_sensor.h"

PHSensor::PHSensor(uint8_t pin) {
    _pin = pin;
}

void PHSensor::begin() {
    pinMode(_pin, INPUT);
}

float PHSensor::readVoltage() {
    const int samples = 20;
    uint32_t totalMv = 0;

    for (int i = 0; i < samples; i++) {
        totalMv += analogReadMilliVolts(_pin);
        delay(10);
    }

    return (totalMv / (float)samples) / 1000.0;
}

float PHSensor::readPH() {
    float voltage = readVoltage();

    // TẠM THỜI.
    // Sau đó chúng ta sẽ thay bằng hệ số calibration pH 4 / pH 7.
    float ph = 7.0 + ((1.50 - voltage) / 0.18);

    return ph;
}