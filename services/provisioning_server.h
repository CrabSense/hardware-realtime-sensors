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
    bool restartPending = false;
    unsigned long restartAt = 0;

    void sendCors();
    void handleInfo();
    void handleProvision();
    void handleNotFound();
};
