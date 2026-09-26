#include "provisioning_server.h"
#include "../config/settings.h"

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
    server.onNotFound([this]() { handleNotFound(); });

    server.begin();

    Serial.println("Provisioning HTTP server started");
}

void ProvisioningServer::loop()
{
    server.handleClient();

    if (restartPending && millis() >= restartAt)
    {
        Serial.println("Restarting to apply WiFi config...");
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
    password.trim();
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
        return;
    }

    wifi->saveCredentials(ssid, password, backendUrl);

    JsonDocument response;
    response["success"] = true;
    response["message"] = "WiFi configuration saved";
    response["deviceCode"] = wifi->getDeviceCode();
    response["deviceId"] = wifi->getDeviceId();
    response["mac"] = wifi->getMacAddress();

    String body;
    serializeJson(response, body);
    server.send(200, "application/json", body);

    restartPending = true;
    restartAt = millis() + 1200;
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
