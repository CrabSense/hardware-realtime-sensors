#include "sensor_manager.h"
#include "../config/pins.h"

SensorManager::SensorManager()
    : phSensor(PH_PIN),
      tdsSensor(TDS_PIN) {}

void SensorManager::begin() {

    temperatureSensor.begin();
    phSensor.begin();
    tdsSensor.begin();
}

SensorData SensorManager::read() {

    SensorData data;

    data.temperature =
        temperatureSensor.readTemperature();

    data.ph =
        phSensor.readPH();

    data.tds =
        tdsSensor.readTDS(data.temperature);

    return data;
}