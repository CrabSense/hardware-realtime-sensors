#include "sensor_manager.h"
#include "../config/pins.h"
#include "../config/settings.h"

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
    data.phMilliVolts =
        phSensor.lastMilliVolts();

    data.ecMs =
        tdsSensor.readEcMs(data.temperature);
    data.tdsPpt =
        data.ecMs * EC_TO_PPT;
    data.tdsPpm =
        data.tdsPpt * 1000.0f;
    data.tdsMilliVolts =
        tdsSensor.lastMilliVolts();
    data.salinitySaturated =
        tdsSensor.isSaturated();

    return data;
}