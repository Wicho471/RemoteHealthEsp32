#include "WebServerService.h"

// HTML incrustado en flash
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <title>MyHealthLink</title>
  <style>
    body { font-family: Arial, sans-serif; background: #f4f4f4; text-align: center; }
    h1 { color: #2c3e50; }
    button { padding: 10px 20px; margin: 10px; font-size: 16px; }
  </style>
</head>
<body>
  <h1>Bienvenido a MyHealthLink</h1>
  <p>Portal cautivo del ESP32</p>
  <button onclick="alert('OK')">Probar</button>
</body>
</html>
)rawliteral";


WebServerService::WebServerService(WifiManager* wifiMgr, uint16_t httpPort, uint16_t dnsPort)
    : wifiMgr(wifiMgr), server(httpPort), dnsPort(dnsPort) {}

bool WebServerService::begin() {
    dnsServer.start(dnsPort, "*", wifiMgr->getSoftAPIP());

    setupFileRoutes();
    setupCaptivePortal();

    server.begin();
    Logger::log("[WebServerService] Servidor web iniciado en (IP: %s)", wifiMgr->getSoftAPIP().toString().c_str());

    return true;
}

void WebServerService::loop() {
    dnsServer.processNextRequest();
}

void WebServerService::setupFileRoutes() {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send_P(200, "text/html", index_html);
    });

    server.onNotFound([](AsyncWebServerRequest* request) {
        request->send_P(200, "text/html", index_html);
    });
}

void WebServerService::setupCaptivePortal() {
    server.on("/generate_204", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->redirect("/");
    });
    server.on("/fwlink", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->redirect("/");
    });
}
