#ifndef BLUETOOTHSERVICE_H
#define BLUETOOTHSERVICE_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "src/managers/CommandManager.h"

class BluetoothService : public BLECharacteristicCallbacks {
public:
    explicit BluetoothService(CommandManager* cmdManager);
    bool begin(const String& deviceName);

protected:
    void onWrite(BLECharacteristic* characteristic) override;

private:
    void sendMessage(const String& message);

    CommandManager* cmdManager;
    BLEServer* server;
    BLEService* service;
    BLECharacteristic* characteristicRX;
    BLECharacteristic* characteristicTX;

    String buffer;
    bool deviceConnected;
};

#endif
