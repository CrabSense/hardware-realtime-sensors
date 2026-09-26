#include "ph_sensor.h"
#include "../config/settings.h"

PHSensor::PHSensor(uint8_t pin) {
    _pin = pin;
}

void PHSensor::begin() {
    pinMode(_pin, INPUT);
    analogSetPinAttenuation(_pin, ADC_11db);
}

float PHSensor::readMilliVolts() {
    const int samples = 20;
    uint32_t totalMv = 0;

    for (int i = 0; i < samples; i++) {
        totalMv += analogReadMilliVolts(_pin);
        delay(10);
    }

    _lastMv = totalMv / (float)samples;
    return _lastMv;
}

float PHSensor::readVoltage() {
    return readMilliVolts() / 1000.0f;
}

float PHSensor::readPH() {
    const float voltageMv = readMilliVolts();
    return PH_CAL_PH + (PH_CAL_MV - voltageMv) / PH_MV_PER_PH;
}
