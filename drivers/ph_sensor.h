#pragma once
#include <Arduino.h>

class PHSensor {
public:
    PHSensor(uint8_t pin);

    void begin();
    float readVoltage();
    float readMilliVolts();
    float readPH();
    float lastMilliVolts() const { return _lastMv; }

private:
    uint8_t _pin;
    float _lastMv = 0;
};