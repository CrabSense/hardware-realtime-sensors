#pragma once
#include <Arduino.h>

class TDSSensor {
public:
    TDSSensor(uint8_t pin);

    void begin();

    float readVoltage();
    float readTDS(float temperature);

private:
    uint8_t _pin;
};