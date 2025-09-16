#include "SerialManager.h"

SerialManager::SerialManager(CommandManager* commandMgr) : commandMgr(commandMgr) {}

void SerialManager::begin(unsigned long baudRate) {
    Serial.begin(baudRate);
    delay(1000);
}

void SerialManager::keepAlive() {
    while (Serial.available() > 0) {
        char c = (char)Serial.read();
        Serial.print(c);
        if (c == '\n') {
            if (inputBuffer.length() > 0) {
                String response = commandMgr->processCommand(inputBuffer);
                Logger::log(response.c_str());
            }
            inputBuffer = "";
        } else {
            inputBuffer += c;
        }
    }
}