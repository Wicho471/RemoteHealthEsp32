#ifndef COMMAND_MANAGER_H
#define COMMAND_MANAGER_H

#include <Arduino.h>
#include "NetworkManager.h"
#include "SensorManager.h"
#include "src/utils/Logger.h"
#include "src/config/Keys.h"
#include <FS.h>
#include <LittleFS.h>

class CommandManager {
public:
    CommandManager(WifiManager* wifi, PreferencesManager& prefs, SensorManager* sensors);

    String processCommand(const String& input);

private:
    static const String SUCCESS_MESSAGE;
    static const String FAILURE_MESSAGE;

    WifiManager* wifi;
    PreferencesManager& prefs;
    SensorManager* sensors;

    char buffer[256];  

    String showNetworkStatus();
    String showSensorStatus();
    String showPreferences();
    String setSTACredentials(const String& newSsid, const String& newPassword);
    String setAPCredentials(const String& newSsid, const String& newPassword);
    String setTest(bool newVal);
    String restartSystem();
    String resetStorageFS();
    String resetPreferences();
    String checkSizeFS();
    String ping();

    // Sensor-specific configuration
    String setBrightness(int value);
};

#endif // COMMAND_MANAGER_H
