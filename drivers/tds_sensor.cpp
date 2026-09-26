#include "tds_sensor.h"

TDSSensor::TDSSensor(uint8_t pin) {
    _pin = pin;
}

void TDSSensor::begin() {
    pinMode(_pin, INPUT);
}

float TDSSensor::readVoltage() {
    const int samples = 30;
    uint32_t totalMv = 0;

    for (int i = 0; i < samples; i++) {
        totalMv += analogReadMilliVolts(_pin);
        delay(10);
    }

    return (totalMv / (float)samples) / 1000.0;
}

float TDSSensor::readTDS(float temperature) {

    float voltage = readVoltage();

    // Temperature compensation
    float compensationCoefficient =
        1.0 + 0.02 * (temperature - 25.0);

    float compensationVoltage =
        voltage / compensationCoefficient;

    float tds =
        (133.42 * compensationVoltage * compensationVoltage * compensationVoltage
        - 255.86 * compensationVoltage * compensationVoltage
        + 857.39 * compensationVoltage)
        * 0.5;

    return tds;
}