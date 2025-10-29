#ifndef WEBSOCKETSERVICE_H
#define WEBSOCKETSERVICE_H

#include <AsyncTCP.h> 
#include <ESPAsyncWebServer.h>
#include "src/managers/CommandManager.h"
#include "src/managers/CertificateManager.h"
#include "src/utils/Logger.h"
#include "src/config/Constans.h" // Asegúrate de definir QUEUE_SIZE y PAYLOAD_MAX_SIZE aquí

// --> CAMBIO: Estructura para pasar por la cola
struct WebSocketEventData {
    uint32_t clientId;
    char payload[PAYLOAD_MAX_SIZE];
};

class WebSocketService {
public:
    explicit WebSocketService(CommandManager* cmdManager, uint16_t port = 443);
    ~WebSocketService();

    bool begin();
    void broadcast(const String& message);
    void sendToClient(uint32_t clientId, const String& message); // --> CAMBIO: Para responder a un cliente específico
    size_t getConnectedClients() const;
    QueueHandle_t getQueueHandle() const; // --> CAMBIO: Getter para la cola

private:
    void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

    QueueHandle_t _queueHandle; // --> CAMBIO: La cola de FreeRTOS
    uint16_t _port;
    CommandManager* _cmdManager;
    CertificateManager _certManager;
    bool _isInitialized;

    AsyncWebServer* _server;
    AsyncWebSocket* _ws;

    const uint8_t MAX_WS_CLIENTS = 3;
};

#endif //WEBSOCKETSERVICE_H