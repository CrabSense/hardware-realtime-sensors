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

    /// Thử Wi-Fi mới. Fail thì giữ SSID/password cũ.
    bool applyCredentials(
        const String& newSsid,
        const String& newPassword,
        const String& newBackendUrl = ""
    );

    void clearCredentials();

    String getDeviceId();
    String getApName();
    String getMacAddress();
    String getBackendUrl();
    String getDeviceCode();
    String getSsid();
    String getLastError();

private:
    Preferences preferences;

    String ssid;
    String password;
    String backendUrl;
    String lastError;
    bool provisioningMode = false;

    void startProvisioningAP();
    bool waitForConnection(uint32_t timeoutMs);
};
