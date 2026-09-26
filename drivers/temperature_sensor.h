#pragma once

#include <Arduino.h>
#include <Adafruit_MAX31865.h>

class TemperatureSensor {
public:
    TemperatureSensor();

    bool begin();
    float readTemperature();

private:
    Adafruit_MAX31865 max31865;
};