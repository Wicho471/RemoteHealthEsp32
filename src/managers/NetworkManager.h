#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <HTTPClient.h>
#include "PreferencesManager.h"
#include "src/config/Keys.h"
#include "src/utils/Logger.h"
#include "src/services/TimeService.h"

class WifiManager {
public:
    explicit WifiManager(PreferencesManager* prefsManager, TimeService* timeService);

    void begin();
    void restartConnection();
    void reloadConfig();
    void updateSTAConfig(const String& newSsid, const String& newPassword);
    void updateAPConfig(const String& newSsid, const String& newPassword);
    void disconectWiFi();

    bool addCredential(const String& ssid, const String& password);
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
    IPAddress getSoftAPIP() const;

private:
    PreferencesManager* prefs;
    TimeService* timeService;
    
    String ssid_sta;
    String password_sta;
    String ssid_ap;
    String password_ap;
    bool internet;
    bool staConnection;

    bool connectWiFi();
    bool checkInternet();
};

#endif // WIFI_MANAGER_H
