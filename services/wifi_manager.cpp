#include "wifi_manager.h"
#include "../config/settings.h"

void WifiManager::begin()
{
    preferences.begin("wifi", false);

    ssid = preferences.getString("ssid", "");
    password = preferences.getString("password", "");
    backendUrl = preferences.getString("backend", DEFAULT_BACKEND_URL);
}

bool WifiManager::connect()
{
    if (ssid.length() == 0)
    {
        startProvisioningAP();
        return false;
    }

    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(getApName().c_str());

    WiFi.begin(
        ssid.c_str(),
        password.c_str()
    );

    Serial.print("Connecting WiFi");

    unsigned long start = millis();

    while (
        WiFi.status() != WL_CONNECTED &&
        millis() - start < WIFI_CONNECT_TIMEOUT_MS
    )
    {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        provisioningMode = false;

        WiFi.softAP(getApName().c_str());

        Serial.println();
        Serial.println("WiFi connected");

        Serial.print("IP: ");
        Serial.println(WiFi.localIP());

        Serial.print("Provisioning AP: ");
        Serial.println(getApName());

        return true;
    }

    Serial.println();
    Serial.println("WiFi connection failed");

    startProvisioningAP();

    return false;
}

void WifiManager::startProvisioningAP()
{
    provisioningMode = true;

    WiFi.mode(WIFI_AP);

    String apName = getApName();

    WiFi.softAP(apName.c_str());

    Serial.print("Provisioning AP: ");
    Serial.println(apName);

    Serial.print("IP: ");
    Serial.println(WiFi.softAPIP());
}

void WifiManager::saveCredentials(
    const String& newSSID,
    const String& newPassword,
    const String& newBackendUrl
)
{
    preferences.putString("ssid", newSSID);
    preferences.putString("password", newPassword);

    ssid = newSSID;
    password = newPassword;

    if (newBackendUrl.length() > 0)
    {
        saveBackendUrl(newBackendUrl);
    }
}

void WifiManager::saveBackendUrl(const String& url)
{
    if (url.length() == 0)
    {
        return;
    }

    preferences.putString("backend", url);
    backendUrl = url;
}

void WifiManager::clearCredentials()
{
    preferences.remove("ssid");
    preferences.remove("password");

    ssid = "";
    password = "";
}

bool WifiManager::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

bool WifiManager::isProvisioning()
{
    return provisioningMode;
}

String WifiManager::getDeviceId()
{
    uint64_t chipid = ESP.getEfuseMac();

    char id[17];

    sprintf(
        id,
        "%04X%08X",
        (uint16_t)(chipid >> 32),
        (uint32_t)chipid
    );

    return String(id);
}

String WifiManager::getApName()
{
    String deviceId = getDeviceId();
    return "CrabSense-" + deviceId.substring(deviceId.length() - 4);
}

String WifiManager::getMacAddress()
{
    return WiFi.macAddress();
}

String WifiManager::getBackendUrl()
{
    return backendUrl;
}

String WifiManager::getDeviceCode()
{
    return getApName();
}
