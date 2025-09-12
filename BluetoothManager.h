#ifndef BLUETOOTHMANAGER_H
#define BLUETOOTHMANAGER_H

#include <BluetoothSerial.h>
#include <Arduino.h>
#include "CommandManager.h"

/**
 * @brief Clase para manejar la comunicación Bluetooth con ESP32 de manera no bloqueante.
 */
class BluetoothManager {
public:
    /**
     * @brief Constructor por defecto.
     */
    BluetoothManager(CommandManager *cmdManager);

    /**
     * @brief Inicializa el Bluetooth con un nombre de dispositivo.
     * @param deviceName Nombre con el que se anunciará el ESP32.
     * @return true si la inicialización fue exitosa, false en caso contrario.
     */
    bool begin(const String& deviceName);

    /**
     * @brief Revisa si hay datos disponibles y los almacena en un buffer interno.
     * Debe llamarse frecuentemente dentro del loop.
     */
    void update();

    /**
     * @brief Obtiene el último mensaje recibido completo (terminado en '\n').
     * @return Mensaje recibido o cadena vacía si no hay disponible.
     */
    String getLastMessage();

    /**
     * @brief Envía un mensaje por Bluetooth al dispositivo conectado.
     * @param message Texto a enviar.
     */
    void sendMessage(const String& message);

private:
    CommandManager *cmdManager;
    BluetoothSerial serialBT;
    String buffer;
    String lastMessage;
};

#endif
