#include "wifi_manager.h"
#include "../config/settings.h"

void WifiManager::begin()
{
    preferences.begin("wifi", false);

    ssid = preferences.getString("ssid", "");
    password = preferences.getString("password", "");
    backendUrl = preferences.getString("backend", DEFAULT_BACKEND_URL);
}

bool WifiManager::waitForConnection(uint32_t timeoutMs)
{
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < timeoutMs)
    {
        delay(400);
        Serial.print(".");
    }
    Serial.println();
    return WiFi.status() == WL_CONNECTED;
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

    Serial.print("Connecting WiFi SSID=");
    Serial.println(ssid);

    WiFi.begin(ssid.c_str(), password.c_str());

    if (waitForConnection(WIFI_CONNECT_TIMEOUT_MS))
    {
        provisioningMode = false;
        lastError = "";
        WiFi.softAP(getApName().c_str());
        Serial.print("WiFi connected IP=");
        Serial.println(WiFi.localIP());
        return true;
    }

    lastError = "Không kết nối được Wi-Fi " + ssid;
    Serial.print("WiFi connection failed status=");
    Serial.println(static_cast<int>(WiFi.status()));
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

bool WifiManager::applyCredentials(
    const String& newSsid,
    const String& newPassword,
    const String& newBackendUrl
)
{
    const String oldSsid = ssid;
    const String oldPassword = password;
    const String oldBackend = backendUrl;

    Serial.print("Apply WiFi SSID=");
    Serial.println(newSsid);

    saveCredentials(newSsid, newPassword, newBackendUrl);

    WiFi.disconnect(false, false);
    delay(250);
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(getApName().c_str());
    WiFi.begin(newSsid.c_str(), newPassword.c_str());

    if (waitForConnection(WIFI_CONNECT_TIMEOUT_MS))
    {
        provisioningMode = false;
        lastError = "";
        Serial.print("WiFi switched IP=");
        Serial.println(WiFi.localIP());
        return true;
    }

    lastError = "Không vào được Wi-Fi \"" + newSsid +
                "\". Sai mật khẩu, SSID, hoặc mạng 5GHz (ESP32 chỉ 2.4GHz).";
    Serial.println(lastError);

    if (oldSsid.length() == 0)
    {
        startProvisioningAP();
        return false;
    }

    Serial.print("Rollback to SSID=");
    Serial.println(oldSsid);
    saveCredentials(oldSsid, oldPassword, oldBackend);
    WiFi.disconnect(false, false);
    delay(250);
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(getApName().c_str());
    WiFi.begin(oldSsid.c_str(), oldPassword.c_str());

    if (waitForConnection(WIFI_CONNECT_TIMEOUT_MS))
    {
        provisioningMode = false;
        Serial.print("Rolled back IP=");
        Serial.println(WiFi.localIP());
        return false;
    }

    startProvisioningAP();
    return false;
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

String WifiManager::getSsid()
{
    return ssid;
}

String WifiManager::getLastError()
{
    return lastError;
}
