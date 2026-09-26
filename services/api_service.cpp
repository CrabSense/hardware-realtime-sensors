#include "api_service.h"
#include "../config/settings.h"

#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <time.h>

void ApiService::begin(WifiManager* wifiManager, SensorManager* sensorManager)
{
    wifi = wifiManager;
    sensors = sensorManager;
    lastPublishMs = 0;
}

void ApiService::loop()
{
    if (wifi == nullptr || sensors == nullptr)
    {
        return;
    }

    if (!wifi->isConnected())
    {
        return;
    }

    unsigned long now = millis();

    if (lastPublishMs != 0 && now - lastPublishMs < SENSOR_PUBLISH_INTERVAL_MS)
    {
        return;
    }

    lastPublishMs = now;
    publish(sensors->read());
}

String ApiService::isoNow()
{
    time_t now = time(nullptr);
    if (now < 1600000000)
    {
        return "";
    }

    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));
    return String(buf);
}

void ApiService::publish(const SensorData& data)
{
    String base = wifi->getBackendUrl();
    base.trim();

    while (base.endsWith("/"))
    {
        base.remove(base.length() - 1);
    }

    if (base.length() == 0)
    {
        Serial.println("API skip: no backend URL");
        return;
    }

    if (base.indexOf("localhost") >= 0 || base.indexOf("127.0.0.1") >= 0)
    {
        Serial.println("API skip: localhost is the ESP itself — set backend to PC LAN IP");
        return;
    }

    String deviceCode = wifi->getDeviceCode();
    String measuredAt = isoNow();

    JsonDocument doc;
    JsonArray measurements = doc["measurements"].to<JsonArray>();

    auto add = [&](const char* suffix, float value, const char* unit) {
        JsonObject item = measurements.add<JsonObject>();
        item["deviceCode"] = deviceCode;
        item["sensorCode"] = deviceCode + suffix;
        item["value"] = value;
        item["unit"] = unit;
        item["ipAddress"] = WiFi.localIP().toString();
        if (measuredAt.length() > 0)
        {
            item["measuredAt"] = measuredAt;
        }
    };

    add(SENSOR_CODE_TEMP_SUFFIX, data.temperature, "C");
    add(SENSOR_CODE_PH_SUFFIX, data.ph, "pH");
    add(SENSOR_CODE_TDS_SUFFIX, data.tds, "ppm");

    String payload;
    serializeJson(doc, payload);

    String url = base + "/api/iot/sensor-data/batch";

    HTTPClient http;
    WiFiClientSecure secure;
    int code = -1;

    if (url.startsWith("https://"))
    {
        secure.setInsecure();
        http.begin(secure, url);
    }
    else
    {
        http.begin(url);
    }

    http.addHeader("Content-Type", "application/json");
    code = http.POST(payload);

    Serial.print("API ingest ");
    Serial.print(code);
    Serial.print(" ");
    Serial.println(http.getString());

    http.end();
}
