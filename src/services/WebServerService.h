#ifndef WEBSERVICESERVICE_H
#define WEBSERVICESERVICE_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include "src/managers/NetworkManager.h"
#include "src/utils/Logger.h"

class WebServerService {
public:
    explicit WebServerService(WifiManager* wifiMgr, uint16_t httpPort = 80, uint16_t dnsPort = 53);

    bool begin();
    void loop();

private:
    WifiManager* wifiMgr;
    AsyncWebServer server;
    DNSServer dnsServer;
    uint16_t dnsPort;
    void setupFileRoutes();
    void setupCaptivePortal();
    static void dnsTask(void* param);
};

#endif
