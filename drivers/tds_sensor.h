#pragma once
#include <Arduino.h>

/// Analog EC on GPIO5 → salinity ppt (nước mặn). Không dùng đa thức TDS SEN0244.
class TDSSensor {
public:
    TDSSensor(uint8_t pin);

    void begin();

    float readVoltage();
    float lastMilliVolts() const { return _lastMv; }
    bool isSaturated() const;

    /// mS/cm — DFRobot_EC::readEC (voltage mV, K = EC_KVALUE).
    float readEcMs(float temperature);
    /// ppt ≈ 0.64 × EC (mS/cm) ở 25°C (PSS-78 gần đúng 10–35 ppt).
    float readSalinityPpt(float temperature);

private:
    uint8_t _pin;
    float _lastMv = 0;
};
