#ifndef SERIALMANAGER_H
#define SERIALMANAGER_H

#include <Arduino.h>
#include "CommandManager.h"

class SerialManager {
public:
    SerialManager(CommandManager* commandMgr);
    void begin(unsigned long baudRate = 115200);
    void keepAlive();
private:
    CommandManager* commandMgr;
    String inputBuffer;
};

#endif
