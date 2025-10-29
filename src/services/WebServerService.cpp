// // #include "src/config/build_config.h"
// #include "WebServerService.h"

// WebServerService::WebServerService(WifiManager* wifiMgr, uint16_t httpPort, uint16_t dnsPort)
//     : wifiMgr(wifiMgr), server(httpPort), dnsPort(dnsPort) {}

// bool WebServerService::begin() {
    

//     dnsServer.start(dnsPort, "*", wifiMgr->getSoftAPIP());

//     setupFileRoutes();
//     setupCaptivePortal();

//     server.begin();
//     Logger::log("[WebServerService] Servidor web iniciado en (IP: %s)", wifiMgr->getSoftAPIP().toString().c_str());

//     return true;
// }

// void WebServerService::keepAlive() {
//     dnsServer.processNextRequest();
// }

// void WebServerService::setupFileRoutes() {
//     server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
//         Logger::log("Se accedió a la página principal\n");
//         request->send_P(200, "text/html", INDEX_HTML);
//     });

//     server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest* request) {
//         Logger::log("Cargando la hoja de estilos\n");
//         request->send_P(200, "text/css", STYLES_CSS);
//     });

//     server.on("/main.js", HTTP_GET, [](AsyncWebServerRequest* request) {
//         Logger::log("Cargando el script principal\n");
//         request->send_P(200, "application/javascript", MAIN_JS);
//     });

//     server.on("/qrcode.min.js.gz", HTTP_GET, [](AsyncWebServerRequest* request) {
//         Logger::log("Cargando el libreria de QRCode\n");
//         if (!LittleFS.begin()) {
//             Logger::log("Error al montar LittleFS\n");
//             request->send(500, "text/plain", "Error al montar LittleFS");
//             return;
//         }

//         if (LittleFS.exists("/qrcode.min.js.gz")){
//             Logger::log("Se encontro el archivo qrcode.min.js.gz\n");
//             AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/qrcode.min.js.gz", "application/javascript");
//             response->addHeader("Content-Encoding", "gzip");
//             request->send(response);
//         } else {
//             Logger::log("No se encontro el archivo qrcode.min.js.gz\n");
//             request->send(404, "text/plain", "Archivo no encontrado");
//         }
//     });

//     server.onNotFound([](AsyncWebServerRequest* request) {
//         Logger::log("Se accedió a la página principal desde onNotFound\n");
//         request->send_P(200, "text/html", INDEX_HTML);
//     });

//     server.on("/scan", HTTP_GET, [this](AsyncWebServerRequest* request) {
//         Logger::log("Escaneando redes para mandar al portal cautivo\n");
//         int n = wifiMgr->getWifiOptions();
//         String json = "[";
//         for (int i = 0; i < n; ++i) {
//             if (i) json += ",";
//             json += "\"" + wifiMgr->getWifiSSID(i) + "\"";
//         }
//         json += "]";
//         request->send(200, "application/json", json);
//     });

//     server.on("/connect", HTTP_POST,
//     [this](AsyncWebServerRequest* request) {
//         Logger::log("Estableciendo nuevas credenciales para STA");
//     }
//     ).onBody([this](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t, size_t) {
//         String body;
//         body.reserve(len);
//         for (size_t i = 0; i < len; i++) {
//             body += (char)data[i];
//         }

//         DynamicJsonDocument doc(256);
//         DeserializationError error = deserializeJson(doc, body);

//         if (!error) {
//             String ssid = doc["ssid"].as<String>();
//             String password = doc["password"].as<String>();
//             if (wifiMgr != nullptr) {
//                 wifiMgr->updateSTAConfig(ssid, password); 
//                 request->send(200, "text/plain", "Esto tiene que cambiar " + ssid);
//             } else {
//                 request->send(500, "text/plain", "WifiManager no inicializado");
//             }
//         } else {
//             request->send(400, "text/plain", "Solicitud inválida");
//         }
//     });

//     server.on("/info", HTTP_GET, [this](AsyncWebServerRequest* request) {
//         DynamicJsonDocument doc(256);
//         doc["path"] = "/";
//         doc["port"] = 8080;
//         doc["ipv4"] = wifiMgr->getIPv4();
//         doc["ipv6"] = wifiMgr->getIPv6();
//         doc["name"] = "Esp32";

//         String json;
//         serializeJson(doc, json);
//         request->send(200, "application/json", json);
//     });
// }

// void WebServerService::setupCaptivePortal() {
//     server.on("/generate_204", HTTP_GET, [](AsyncWebServerRequest* request) {
//         request->redirect("/");
//     });
//     server.on("/fwlink", HTTP_GET, [](AsyncWebServerRequest* request) {
//         request->redirect("/");
//     });
// }
