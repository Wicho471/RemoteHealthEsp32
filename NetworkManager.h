#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <HTTPClient.h>
#include "PreferencesManager.h"
#include "config/Keys.h"

class WifiManager {
public:
    explicit WifiManager(PreferencesManager* prefsManager);

    void begin();
    void restartConnection();
    void reloadConfig();
    void updateSTAConfig(const String& newSsid, const String& newPassword);
    void updateAPConfig(const String& newSsid, const String& newPassword);

    bool addCredential (const String& ssid, const String& password);
    bool removeCredential(const String& ssid);
    bool clearVault();

    bool isInternetAvailable() const;
    bool isConnectedToWifi() const;
    String getIPv4() const;
    String getIPv6() const;
    String getSSID_STA() const;
    String getPASS_STA() const;
    String getSSID_AP() const;
    String getPASS_AP() const;

private:
    PreferencesManager* prefs;
    String ssid_sta;
    String password_sta;
    String ssid_ap;
    String password_ap;
    bool internet;
    bool staConnection;

    bool connectWiFi();
    bool checkInternet();
    bool setupTime();
    void loadLastTimeSaved();
};

#endif // WIFI_MANAGER_H
