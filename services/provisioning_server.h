#pragma once

#include <WebServer.h>
#include "wifi_manager.h"

class ProvisioningServer {
public:
    ProvisioningServer();
    void begin(WifiManager* wifi);
    void loop();

private:
    WebServer server;
    WifiManager* wifi = nullptr;
    bool applyPending = false;
    unsigned long applyAt = 0;
    bool restartPending = false;
    unsigned long restartAt = 0;
    String pendingSsid;
    String pendingPassword;
    String pendingBackend;

    void sendCors();
    void handleInfo();
    void handleProvision();
    void handleRestart();
    void handleNotFound();
};
