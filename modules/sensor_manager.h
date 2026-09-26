#pragma once

#include "../drivers/temperature_sensor.h"
#include "../drivers/ph_sensor.h"
#include "../drivers/tds_sensor.h"

struct SensorData {
    float temperature;
    float ph;
    float tds;
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