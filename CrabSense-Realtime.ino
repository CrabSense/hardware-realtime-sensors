#include <Arduino.h>

#include "modules/sensor_manager.h"
#include "services/wifi_manager.h"
#include "services/provisioning_server.h"
#include "services/api_service.h"

SensorManager sensors;
WifiManager wifi;
ProvisioningServer provisioning;
ApiService api;

void printReading(const SensorData& data)
{
    Serial.println("------ SENSOR DATA ------");

    Serial.print("Temperature: ");
    Serial.print(data.temperature, 2);
    Serial.println(" C");

    Serial.print("pH: ");
    Serial.println(data.ph, 2);

    Serial.print("TDS: ");
    Serial.print(data.tds, 0);
    Serial.println(" ppm");

    Serial.println("-------------------------");
    Serial.println();
}

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("==========================");
    Serial.println(" CrabSense Realtime");
    Serial.println("==========================");

    sensors.begin();
    wifi.begin();

    bool online = wifi.connect();

    provisioning.begin(&wifi);
    api.begin(&wifi, &sensors);

    if (online)
    {
        configTime(0, 0, "pool.ntp.org", "time.nist.gov");
        Serial.print("Device: ");
        Serial.println(wifi.getDeviceCode());
        Serial.print("Backend: ");
        Serial.println(wifi.getBackendUrl());
    }
    else
    {
        Serial.println("Waiting for Owner provisioning...");
        Serial.println("Connect to the CrabSense-XXXX AP, then");
        Serial.println("POST http://192.168.4.1/api/provision");
    }
}

void loop()
{
    provisioning.loop();
    api.loop();

    static unsigned long lastPrint = 0;
    if (millis() - lastPrint < 2000)
    {
        return;
    }

    lastPrint = millis();

    if (!wifi.isConnected())
    {
        return;
    }

    printReading(sensors.read());
}
