#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <Preferences.h>

class WifiManager {
public:
    void begin();

    bool connect();
    bool isConnected();
    bool isProvisioning();

    void saveCredentials(
        const String& ssid,
        const String& password,
        const String& backendUrl = ""
    );

    void saveBackendUrl(const String& backendUrl);

    void clearCredentials();

    String getDeviceId();
    String getApName();
    String getMacAddress();
    String getBackendUrl();
    String getDeviceCode();

private:
    Preferences preferences;

    String ssid;
    String password;
    String backendUrl;
    bool provisioningMode = false;

    void startProvisioningAP();
};
