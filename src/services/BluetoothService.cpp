#include "BluetoothService.h"

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

BluetoothService::BluetoothService(CommandManager* cmdManager)
    : cmdManager(cmdManager),
      server(nullptr),
      service(nullptr),
      characteristicRX(nullptr),
      characteristicTX(nullptr),
      buffer(""),
      deviceConnected(false) {}

bool BluetoothService::begin(const String& deviceName) {
    BLEDevice::init(deviceName.c_str());
    server = BLEDevice::createServer();

    service = server->createService(SERVICE_UUID);

    characteristicTX = service->createCharacteristic(
        CHARACTERISTIC_UUID_TX,
        BLECharacteristic::PROPERTY_NOTIFY
    );
    characteristicTX->addDescriptor(new BLE2902());

    characteristicRX = service->createCharacteristic(
        CHARACTERISTIC_UUID_RX,
        BLECharacteristic::PROPERTY_WRITE
    );
    characteristicRX->setCallbacks(this);

    service->start();
    BLEAdvertising* advertising = server->getAdvertising();
    advertising->addServiceUUID(SERVICE_UUID);
    advertising->start();

    return true;
}

void BluetoothService::onWrite(BLECharacteristic* characteristic) {
    auto value = characteristic->getValue();  // std::string
    if (!value.isEmpty()) {
        for (char c : value) {
            if (c == '\n') {
                String response = cmdManager->processCommand(buffer);
                sendMessage(response);
                buffer = "";
            } else {
                buffer += c;
            }
        }
    }
}

void BluetoothService::sendMessage(const String& message) {
    if (characteristicTX != nullptr) {
        characteristicTX->setValue(message.c_str());
        characteristicTX->notify();
    }
}
