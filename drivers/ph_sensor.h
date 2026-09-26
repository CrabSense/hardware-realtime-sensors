#pragma once
#include <Arduino.h>

class PHSensor {
public:
    PHSensor(uint8_t pin);

    void begin();
    float readVoltage();
    float readPH();

private:
    uint8_t _pin;
};