#pragma once

#include "../drivers/temperature_sensor.h"
#include "../drivers/ph_sensor.h"
#include "../drivers/tds_sensor.h"

struct SensorData {
    float temperature;
    float ph;
    float tdsPpm;
    float tdsPpt;
    float phMilliVolts;
    float tdsMilliVolts;
    float ecMs;
    bool salinitySaturated;
};

class SensorManager {
public:
    SensorManager();

    void begin();
    SensorData read();

private:
    TemperatureSensor temperatureSensor;
    PHSensor phSensor;
    TDSSensor tdsSensor;
};