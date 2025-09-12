#include "BluetoothManager.h"

BluetoothManager::BluetoothManager(CommandManager* cmdManager) : cmdManager(cmdManager), buffer(""), lastMessage("") {}

bool BluetoothManager::begin(const String& deviceName) {
    if (!serialBT.begin(deviceName)) {
        Serial.println("Bluetooth init failed.");
        return false;
    }
    Serial.println("Bluetooth initialized as: " + deviceName);
    return true;
}

void BluetoothManager::update() {
    while (serialBT.available()) {
        char c = serialBT.read();
        if (c == '\n') {
            lastMessage = buffer;
            buffer = "";
        } else {
            buffer += c;
        }
    }
}

String BluetoothManager::getLastMessage() {
    String temp = lastMessage;
    lastMessage = "";
    return temp;
}

void BluetoothManager::sendMessage(const String& message) {
    serialBT.println(message);
}
