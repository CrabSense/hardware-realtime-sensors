#include "tds_sensor.h"
#include "../config/settings.h"

TDSSensor::TDSSensor(uint8_t pin) {
    _pin = pin;
}

void TDSSensor::begin() {
    pinMode(_pin, INPUT);
    analogSetPinAttenuation(_pin, ADC_11db);
}

float TDSSensor::readVoltage() {
    const int samples = 30;
    uint32_t totalMv = 0;

    for (int i = 0; i < samples; i++) {
        totalMv += analogReadMilliVolts(_pin);
        delay(10);
    }

    _lastMv = totalMv / (float)samples;
    return _lastMv / 1000.0f;
}

bool TDSSensor::isSaturated() const {
    return _lastMv >= EC_SAT_MV;
}

float TDSSensor::readEcMs(float temperature) {
    readVoltage();

    float t = temperature;
    if (t < 0.0f || t > 40.0f)
    {
        t = 25.0f;
    }

    // DFRobot_EC: voltage is millivolts.
    float rawEc = 1000.0f * _lastMv / EC_RES2 / EC_REF * EC_KVALUE;
    float ec = rawEc * EC_KVALUE;
    return ec / (1.0f + 0.0185f * (t - 25.0f));
}

float TDSSensor::readSalinityPpt(float temperature) {
    return readEcMs(temperature) * EC_TO_PPT;
}
