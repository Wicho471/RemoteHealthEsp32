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
    else if (cmd == "ResetPreferences") return resetPreferences();
    else if (cmd == "ResetStorageFS") return resetStorageFS();
    else if (cmd == "CheckSizeFS") return checkSizeFS();
    else if (cmd == "SetAPCredentials" && count == 3) return setAPCredentials(tokens[1], tokens[2]);
    else if (cmd == "SetSTACredentials" && count == 3) return setSTACredentials(tokens[1], tokens[2]);
    else if (cmd == "SetBrightness" && count == 2) return setBrightness(tokens[1].toInt());
    else if (cmd == "ping") return ping();
    Logger::log("Comando desconocido: %s\n", input.c_str());
    return FAILURE_MESSAGE;
}

String CommandManager::ping() {
    Logger::log("Ping recibido\n");
    snprintf(buffer, sizeof(buffer), "{\"type\":\"PING\"}");
    return String(buffer);
}

String CommandManager::resetStorageFS() {
    if (!LittleFS.begin()) {
        Logger::log("Error al montar LittleFS\n");
        return FAILURE_MESSAGE;
    } else {
        Logger::log("LittleFS montado correctamente\n");
        LittleFS.format();
        delay(10);
        return SUCCESS_MESSAGE;
    }
}
String CommandManager::checkSizeFS() {
    if (!LittleFS.begin()) {
        Logger::log("Error al montar LittleFS\n");
        return FAILURE_MESSAGE;
    } else {
        Logger::log("LittleFS montado correctamente\n");
        uint32_t totalBytes = LittleFS.totalBytes();
        uint32_t usedBytes = LittleFS.usedBytes();
        snprintf(buffer, sizeof(buffer), "{\"total\":%u,\"used\":%u}", totalBytes, usedBytes);
        return String(buffer);
    }
}

String CommandManager::resetPreferences() {
    prefs.clearAll();
    return SUCCESS_MESSAGE;
}


String CommandManager::showNetworkStatus() {
    snprintf(buffer, sizeof(buffer),
        "{\"type\":\"NETWORK_STATUS\",\"STA\":%s,\"internet\":%s,\"ipv4\":\"%s\",\"ipv6\":\"%s\"}",
        wifi->isConnectedToWifi() ? "true" : "false",
        wifi->isInternetAvailable() ? "true" : "false",
        wifi->getIPv4().c_str(),
        wifi->getIPv6().c_str());
    return String(buffer);
}

String CommandManager::showSensorStatus() {
    snprintf(buffer, sizeof(buffer), 
         "{\"type\":\"SENSORS_STATUS\",\"infrarred\":%s,\"oximeter\":%s,\"accelerometer\":%s,\"ecg\":%s}",
        sensors->isMLXEnabled() ? "true" : "false",
        sensors->isMAXEnabled() ? "true" : "false",
        sensors->isACCELEnabled() ? "true" : "false",
        sensors->isEcgConnected() ? "true" : "false");
    return String(buffer);
}

String CommandManager::showPreferences() {
    String ssidSta = wifi->getSSID_STA();
    String passSta = wifi->getPASS_STA();
    String ssidAp = wifi->getSSID_AP();
    String passAp = wifi->getPASS_AP();
    int oxiBrightness = prefs.load<int>(KEY_OXI_BRIGHTNESS, 0);

    snprintf(buffer, sizeof(buffer), 
        "{\"type\":\"PREFERENCES_STATUS\",\"ssidSta\":\"%s\",\"passwordSta\":\"%s\",\"ssidAp\":\"%s\",\"passwordAp\":\"%s\",\"apEnabled\":%s,\"oximeterBrightness\":%d}",
        ssidSta.c_str(), passSta.c_str(),
        ssidAp.c_str(), passAp.c_str(),
        wifi->getSoftAPIP() != IPAddress(0, 0, 0, 0) ? "true" : "false",
        oxiBrightness);
    return String(buffer);
}

String CommandManager::setSTACredentials(const String& newSsid, const String& newPassword) {
    wifi->updateSTAConfig(newSsid, newPassword);
    delay(100);
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
    value = constrain(value, 0, 255);
    sensors->setMAX3010xBrightness(value);
    return SUCCESS_MESSAGE;
}
