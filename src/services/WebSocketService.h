#ifndef WEBSOCKETSERVICE_H
#define WEBSOCKETSERVICE_H

#include <WebSocketsServer.h>
#include <Arduino.h>
#include "src/managers/CommandManager.h"
#include "src/utils/Logger.h"
#include "src/config/Constans.h"

class WebSocketService {
public:
    explicit WebSocketService(CommandManager* cmdManager, uint16_t port = 8080);

    void begin();

    void keepAlive();

    void broadcast(String message);

    int getConnectedClients();

    QueueHandle_t getQueueHandle();

private:
    WebSocketsServer wsServer;
    CommandManager* cmdManager;
    int connectedClients;
    QueueHandle_t queueHandle;

    static void onEventStatic(uint8_t clientId, WStype_t type, uint8_t* payload, size_t length, void* arg);
    void onEvent(uint8_t clientId, WStype_t type, uint8_t* payload, size_t length);
    void sendToClient(uint8_t clientId, String message);
};

#endif
