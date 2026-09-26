#pragma once

#include "wifi_manager.h"
#include "../modules/sensor_manager.h"

class ApiService {
public:
    void begin(WifiManager* wifi, SensorManager* sensors);
    void loop();

private:
    WifiManager* wifi = nullptr;
    SensorManager* sensors = nullptr;
    unsigned long lastPublishMs = 0;

    void publish(const SensorData& data);
    String isoNow();
};
