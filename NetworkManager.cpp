#include "NetworkManager.h"
#include "PreferencesManager.h"
#include "time.h"

constexpr const char* URL_CONNECTIVITY_CHECK = "http://clients3.google.com/generate_204";
const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const char* ntpServer3 = "time.google.com";
const long gmtOffset_sec = -6 * 3600;
const int daylightOffset_sec = 0;

WifiManager::WifiManager(PreferencesManager* prefsManager)
    : prefs(prefsManager), internet(false), staConnection(false) {}

void WifiManager::begin() {
    reloadConfig();
    WiFi.enableIPv6();
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(ssid_ap.c_str(), password_ap.c_str());
    restartConnection();
}

void WifiManager::reloadConfig() {
    ssid_sta = prefs->load<String>(KEY_SSID_STA, "default_sta");
    password_sta = prefs->load<String>(KEY_PASSWORD_STA, "default_pass");
    ssid_ap = prefs->load<String>(KEY_SSID_AP, "ESP32_AP");
    password_ap = prefs->load<String>(KEY_PASSWORD_AP, "12345678");
}

bool WifiManager::connectWiFi() {
    WiFi.disconnect(true);
    delay(1000);
    WiFi.begin(ssid_sta.c_str(), password_sta.c_str());

    unsigned long t0 = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - t0 < 15000) {
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

bool WifiManager::setupTime() {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2, ntpServer3);
    struct tm timeinfo;

    for (int i = 0; i < 10; i++) {
        if (getLocalTime(&timeinfo)) {
            time_t now;
            time(&now);
            prefs->save(KEY_TIME, static_cast<uint64_t>(now));
            return true;
        }
        delay(1000);
    }

    loadLastTimeSaved();
    return false;
}

void WifiManager::loadLastTimeSaved() {
    uint64_t saved = prefs->load<uint64_t>(KEY_TIME, 0);
    if (saved > 0) {
        struct timeval tv = { .tv_sec = static_cast<time_t>(saved), .tv_usec = 0 };
        settimeofday(&tv, nullptr);
    }
}

void WifiManager::restartConnection() {
    if (!connectWiFi()) {
        internet = false;
        staConnection = false;
        return;
    }

    staConnection = true;

    if (!checkInternet()) {
        internet = false;
        return;
    }

    if (!setupTime()) {
        internet = false;
        return;
    }

    internet = true;
}

void WifiManager::updateSTAConfig(const String& newSsid, const String& newPassword) {
    prefs->save(KEY_SSID_STA, newSsid);
    prefs->save(KEY_PASSWORD_STA, newPassword);
    reloadConfig();
    restartConnection();
}

void WifiManager::updateAPConfig(const String& newSsid, const String& newPassword) {
    prefs->save(KEY_SSID_AP, newSsid);
    prefs->save(KEY_PASSWORD_AP, newPassword);
    reloadConfig();
    WiFi.softAPdisconnect(true);
    delay(500);
    WiFi.softAP(ssid_ap.c_str(), password_ap.c_str());
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
