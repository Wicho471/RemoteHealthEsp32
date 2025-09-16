#ifndef WEBSERVICESERVICE_H
#define WEBSERVICESERVICE_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <LittleFS.h>
#include "src/managers/NetworkManager.h"
#include "src/utils/Logger.h"
#include "src/web/WebContent.h"

class WebServerService {
public:
    explicit WebServerService(WifiManager* wifiMgr, uint16_t httpPort = 80, uint16_t dnsPort = 53);

    bool begin();
    void keepAlive();

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
