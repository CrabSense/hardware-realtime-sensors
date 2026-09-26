#include "provisioning_server.h"
#include "../config/settings.h"
#include "../config/pins.h"

#include <ArduinoJson.h>
#include <WiFi.h>

ProvisioningServer::ProvisioningServer()
    : server(PROVISION_HTTP_PORT)
{
}

void ProvisioningServer::sendCors()
{
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
    server.sendHeader("Connection", "close");
}

void ProvisioningServer::begin(WifiManager* wifiManager)
{
    wifi = wifiManager;

    server.on("/api/info", HTTP_GET, [this]() { handleInfo(); });
    server.on("/api/provision", HTTP_OPTIONS, [this]() {
        sendCors();
        server.send(204);
    });
    server.on("/api/provision", HTTP_POST, [this]() { handleProvision(); });
    server.on("/api/restart", HTTP_OPTIONS, [this]() {
        sendCors();
        server.send(204);
    });
    server.on("/api/restart", HTTP_POST, [this]() { handleRestart(); });
    server.on("/api/reboot", HTTP_POST, [this]() { handleRestart(); });
    server.onNotFound([this]() { handleNotFound(); });

    server.begin();
    Serial.println("Provisioning HTTP server started");
}

void ProvisioningServer::loop()
{
    server.handleClient();

    if (applyPending && millis() >= applyAt)
    {
        applyPending = false;
        const bool ok = wifi->applyCredentials(
            pendingSsid,
            pendingPassword,
            pendingBackend
        );
        Serial.println(ok ? "WiFi apply ok" : "WiFi apply failed, kept previous network if possible");
        pendingSsid = "";
        pendingPassword = "";
        pendingBackend = "";
    }

    if (restartPending && millis() >= restartAt)
    {
        Serial.println("Restarting by control channel...");
        ESP.restart();
    }
}

void ProvisioningServer::handleInfo()
{
    sendCors();

    JsonDocument doc;
    doc["deviceId"] = wifi->getDeviceId();
    doc["deviceCode"] = wifi->getDeviceCode();
    doc["apName"] = wifi->getApName();
    doc["controllerType"] = CONTROLLER_TYPE;
    doc["firmware"] = FIRMWARE_VERSION;
    doc["mac"] = wifi->getMacAddress();
    doc["provisioned"] = wifi->isConnected();
    doc["staIp"] = WiFi.localIP().toString();
    doc["backendUrl"] = wifi->getBackendUrl();
    doc["wifiSsid"] = wifi->getSsid();
    doc["lastError"] = wifi->getLastError();

    JsonArray sensors = doc["sensors"].to<JsonArray>();
    auto addSensor = [&](
        const char* suffix,
        const char* type,
        const char* unit,
        const char* iface,
        int gpio,
        const char* channel
    ) {
        JsonObject s = sensors.add<JsonObject>();
        s["suffix"] = suffix;
        s["sensorCode"] = wifi->getDeviceCode() + suffix;
        s["sensorType"] = type;
        s["unit"] = unit;
        s["interface"] = iface;
        s["gpio"] = gpio;
        s["channel"] = channel;
    };
    addSensor(SENSOR_CODE_TEMP_SUFFIX, "Temperature", "C", "SPI", MAX31865_CS, "MAX31865 CS");
    addSensor(SENSOR_CODE_PH_SUFFIX, "pH", "pH", "ADC", PH_PIN, "GPIO4");
    addSensor(SENSOR_CODE_TDS_SUFFIX, "Salinity", "ppt", "ADC", SALINITY_PIN, "GPIO5");
    doc["sensorCount"] = sensors.size();
    doc["outputCount"] = 0;

    String body;
    serializeJson(doc, body);
    server.send(200, "application/json", body);
}

void ProvisioningServer::handleProvision()
{
    sendCors();

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, server.arg("plain"));

    if (err)
    {
        server.send(
            400,
            "application/json",
            "{\"success\":false,\"message\":\"Invalid JSON\"}"
        );
        return;
    }

    String ssid = doc["ssid"] | "";
    String password = doc["password"] | "";
    String backendUrl = doc["backendUrl"] | "";

    ssid.trim();
    backendUrl.trim();

    if (ssid.length() == 0)
    {
        if (backendUrl.length() == 0)
        {
            server.send(
                400,
                "application/json",
                "{\"success\":false,\"message\":\"ssid is required\"}"
            );
            return;
        }

        wifi->saveBackendUrl(backendUrl);

        JsonDocument response;
        response["success"] = true;
        response["message"] = "Backend URL saved";
        response["deviceCode"] = wifi->getDeviceCode();
        response["backendUrl"] = wifi->getBackendUrl();

        String body;
        serializeJson(response, body);
        server.send(200, "application/json", body);
        server.client().stop();
        return;
    }

    pendingSsid = ssid;
    pendingPassword = password;
    pendingBackend = backendUrl;
    applyPending = true;
    applyAt = millis() + 400;

    JsonDocument response;
    response["success"] = true;
    response["message"] = "Da nhan Wi-Fi, dang thu ket noi. Neu that bai se giu mang cu.";
    response["deviceCode"] = wifi->getDeviceCode();
    response["deviceId"] = wifi->getDeviceId();
    response["mac"] = wifi->getMacAddress();
    response["ssid"] = ssid;

    String body;
    serializeJson(response, body);
    server.send(200, "application/json", body);
    server.client().stop();
}

void ProvisioningServer::handleRestart()
{
    sendCors();
    restartPending = true;
    restartAt = millis() + 600;
    server.send(
        200,
        "application/json",
        "{\"success\":true,\"message\":\"Restarting\"}"
    );
    server.client().stop();
}

void ProvisioningServer::handleNotFound()
{
    if (server.method() == HTTP_OPTIONS)
    {
        sendCors();
        server.send(204);
        return;
    }

    sendCors();
    server.send(404, "application/json", "{\"success\":false,\"message\":\"Not found\"}");
}
