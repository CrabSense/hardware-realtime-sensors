#include "temperature_sensor.h"
#include "../config/pins.h"

#define RREF      430.0
#define RNOMINAL  100.0

TemperatureSensor::TemperatureSensor()
    : max31865(
        MAX31865_CS,
        MAX31865_MOSI,
        MAX31865_MISO,
        MAX31865_SCK
      ) {}

bool TemperatureSensor::begin() {
    // PT100 3 dây
    return max31865.begin(MAX31865_3WIRE);
}

float TemperatureSensor::readTemperature() {
    return max31865.temperature(RNOMINAL, RREF);
}