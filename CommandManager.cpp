#include "CommandManager.h"
#include "PreferencesManager.h"

const String CommandManager::SUCCESS_MESSAGE = "{\"message\":\"success\"}";
const String CommandManager::FAILURE_MESSAGE = "{\"message\":\"failure\"}";

CommandManager::CommandManager(WifiManager* wifi, PreferencesManager& prefs, SensorManager* sensors)
    : wifi(wifi), prefs(prefs), sensors(sensors)  {}

String CommandManager::processCommand(const String& input) {
    String tokens[4];
    int count = 0;
    String copy = input;
    copy.trim();

    while (copy.length() > 0 && count < 4) {
        int spaceIndex = copy.indexOf(' ');
        if (spaceIndex == -1) {
            tokens[count++] = copy;
            break;
        } else {
            tokens[count++] = copy.substring(0, spaceIndex);
            copy = copy.substring(spaceIndex + 1);
            copy.trim();
        }
    }

    if (count == 0) return FAILURE_MESSAGE;

    String cmd = tokens[0];

    if (cmd == "ShowNetworkStatus") return showNetworkStatus();
    else if (cmd == "ShowSensorStatus") return showSensorStatus();
    else if (cmd == "ShowPreferences") return showPreferences();
    else if (cmd == "RestartSystem") return restartSystem();
    else if (cmd == "SetAPCredentials" && count == 3) return setAPCredentials(tokens[1], tokens[2]);
    else if (cmd == "SetSTACredentials" && count == 3) return setSTACredentials(tokens[1], tokens[2]);
    else if (cmd == "SetBrightness" && count == 2) return setBrightness(tokens[1].toInt());
    return FAILURE_MESSAGE;
}

String CommandManager::showNetworkStatus() {
    snprintf(buffer, sizeof(buffer),
        "{\"STA\":%s,\"internet\":%s,\"ipv4\":\"%s\",\"ipv6\":\"%s\"}",
        wifi->isConnectedToWifi() ? "true" : "false",
        wifi->isInternetAvailable() ? "true" : "false",
        wifi->getIPv4().c_str(),
        wifi->getIPv6().c_str());
    return String(buffer);
}

String CommandManager::showSensorStatus() {
    snprintf(buffer, sizeof(buffer), 
         "{\"infrarred\":%s,\"oximeter\":%s,\"accelerometer\":%s,\"ecg\":%s}",
        sensors->isMLXReady() ? "true" : "false",
        sensors->isMAXReady() ? "true" : "false",
        sensors->isACCELReady() ? "true" : "false",
        sensors->isEcgConnected() ? "true" : "false");
    return String(buffer);
}

String CommandManager::showPreferences() {
    String ssidSta = wifi->getSSID_STA();
    String passSta = wifi->getPASS_STA();
    String ssidAp = wifi->getSSID_AP();
    String passAp = wifi->getPASS_AP();
    String oxiBrightness = prefs.load<String>(KEY_OXI_BRIGHTNESS, "0");
    uint64_t timestamp = prefs.load<uint64_t>(KEY_TIME, 0);

    snprintf(buffer, sizeof(buffer), 
        "{\"ssidSta\":\"%s\",\"passwordSta\":\"%s\",\"ssidAp\":\"%s\",\"passwordAp\":\"%s\",\"apEnabled\":%s,\"oximeterBrightness\":%s,\"timestamp\":%llu}",
        ssidSta.c_str(), passSta.c_str(),
        ssidAp.c_str(), passAp.c_str(),
        timestamp);
    return String(buffer);
}

String CommandManager::setSTACredentials(const String& newSsid, const String& newPassword) {
    wifi->updateSTAConfig(newSsid, newPassword);
    return showNetworkStatus();
}

String CommandManager::setAPCredentials(const String& newSsid, const String& newPassword) {
    wifi->updateAPConfig(newSsid, newPassword);
    return showPreferences();
}

String CommandManager::restartSystem() {
    ESP.restart();
    return SUCCESS_MESSAGE;
}

String CommandManager::setBrightness(int value) {
    bool maxOK = sensors->isMAXReady();
    if (!maxOK) {
        return FAILURE_MESSAGE;
    }

    value = constrain(value, 0, 255);
    sensors->setMAX3010xBrightness(value);
    return SUCCESS_MESSAGE;
}
