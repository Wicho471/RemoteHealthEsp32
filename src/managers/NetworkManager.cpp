#include "NetworkManager.h"
#include "PreferencesManager.h"
#include "time.h"

constexpr const char* URL_CONNECTIVITY_CHECK = "http://clients3.google.com/generate_204";

WifiManager::WifiManager(PreferencesManager* prefsManager, TimeService* timeService)
    : prefs(prefsManager), timeService(timeService) , internet(false), staConnection(false) {}

void WifiManager::begin() {
    Logger::log("Inciando WifiManager\n");
    reloadConfig();
    Logger::log(WiFi.enableIPv6() ? "IPv6 habilitado correctamente\n" : "No se pudo habilitar IPv6\n");
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(ssid_ap.c_str(), password_ap.c_str());
    restartConnection();
}

void WifiManager::reloadConfig() {
    ssid_sta = prefs->load<String>(KEY_SSID_STA, "Totalplay-2.4G-3290");
    password_sta = prefs->load<String>(KEY_PASSWORD_STA, "nc8anF2tNfPNGUqQ");
    ssid_ap = prefs->load<String>(KEY_SSID_AP, "ESP32_AP");
    password_ap = prefs->load<String>(KEY_PASSWORD_AP, "12345678");
}

bool WifiManager::connectWiFi() {
    Logger::log("Iniciando conexion a WiFi con SSID: %s y password: %s\n", ssid_sta.c_str(), password_sta.c_str());
    unsigned long t0 = millis();
    WiFi.begin(ssid_sta.c_str(), password_sta.c_str());
    Logger::log("Conectando a WiFi");
    while (WiFi.status() != WL_CONNECTED && millis() - t0 < 15000) {
        Logger::log(".");
        delay(500);
    }

    return WiFi.status() == WL_CONNECTED;
}

bool WifiManager::checkInternet() {
    HTTPClient http;
    http.begin(URL_CONNECTIVITY_CHECK);
    int httpCode = http.GET();
    http.end();
    return (httpCode == 204);
}

void WifiManager::restartConnection() {    
    if (!connectWiFi()) {
        Logger::log("\nNo se pudo conectar a WiFi \n");
        internet = false;
        staConnection = false;
        return;
    }
    Logger::log("\nConexion establecida a WiFi\n");
    Logger::log("Ipv4: %s\n", getIPv4().c_str());
    
    Logger::log(WiFi.enableIPv6() ? "IPv6 habilitado correctamente\n" : "No se pudo habilitar IPv6\n");
    Logger::log("Conectando a IPv6");
    unsigned long tIPv6 = millis();
    while (getIPv6() == "::" && millis() - tIPv6 < 15000) {
        Logger::log(".");
        delay(1000);
    }
    Logger::log("\nIpv6: %s\n", getIPv6().c_str());
    
    staConnection = true;

    if (!checkInternet()) {
        Logger::log("No hay conexion a internet\n");
        internet = false;
        return;
    }
    Logger::log("Conexion a internet establecida\n");

    if (!timeService->setupTime()) {
        internet = false;
        return;
    }
    internet = true;
}

void WifiManager::updateSTAConfig(const String& newSsid, const String& newPassword) {
    Serial.println("Actualizando configuracion de WiFi STA");
    prefs->save(KEY_SSID_STA, newSsid);
    prefs->save(KEY_PASSWORD_STA, newPassword);
    reloadConfig();
    disconectWiFi();
    restartConnection();
}

void WifiManager::updateAPConfig(const String& newSsid, const String& newPassword) {
    Serial.println("Actualizando configuracion de WiFi AP");
    prefs->save(KEY_SSID_AP, newSsid);
    prefs->save(KEY_PASSWORD_AP, newPassword);
    reloadConfig();
    WiFi.softAPdisconnect(true);
    delay(500);
    WiFi.softAP(ssid_ap.c_str(), password_ap.c_str());
}

void WifiManager::disconectWiFi() {
    if (!(WiFi.status() == WL_CONNECTED)) return;
    WiFi.disconnect();
    delay(500);
    staConnection = false;
    internet = false;
    Logger::log("Desconectado del WiFi\n");
}

int WifiManager::getWifiOptions() {
    return WiFi.scanNetworks();
}

String WifiManager::getWifiSSID(int index) {
    return WiFi.SSID(index);
}

bool WifiManager::isInternetAvailable() const {
    return internet;
}

bool WifiManager::isConnectedToWifi() const {
    return staConnection;
}

String WifiManager::getIPv4() const {
    return WiFi.localIP().toString();
}

String WifiManager::getIPv6() const {
    return WiFi.globalIPv6().toString();
}

String WifiManager::getSSID_STA() const {
    return ssid_sta;
}

String WifiManager::getPASS_STA() const {
    return password_sta;
}

String WifiManager::getSSID_AP() const {
    return ssid_ap;
}

String WifiManager::getPASS_AP() const {
    return password_ap;
}

IPAddress WifiManager::getSoftAPIP() const {
    return WiFi.softAPIP();
}
