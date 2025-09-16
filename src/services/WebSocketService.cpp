#include "WebSocketService.h"

WebSocketService::WebSocketService(CommandManager* cmdManager ,uint16_t port)
    : wsServer(port), cmdManager(cmdManager), connectedClients(0), queueHandle(nullptr) {}

void WebSocketService::begin() {
    wsServer.begin();
    wsServer.onEvent([this](uint8_t clientId, WStype_t type, uint8_t* payload, size_t length) {
        onEventStatic(clientId, type, payload, length, this);
    });

    queueHandle = xQueueCreate(QUEUE_SIZE, sizeof(char[PAYLOAD_MAX_SIZE]));
    Logger::log(queueHandle == nullptr
        ? "\n[WebSocket] No se pudo crear la cola de mensajes\n"
        : "\n[WebSocket] Cola de mensajes creada\n");
}

void WebSocketService::keepAlive() {
    wsServer.loop();
}

void WebSocketService::broadcast(String message) {
    wsServer.broadcastTXT(message);
}

void WebSocketService::sendToClient(uint8_t clientId, String message) {
    wsServer.sendTXT(clientId, message);
}

int WebSocketService::getConnectedClients() {
    return wsServer.connectedClients();
}

QueueHandle_t WebSocketService::getQueueHandle() {
    return queueHandle;
}

void WebSocketService::onEventStatic(uint8_t clientId, WStype_t type, uint8_t* payload, size_t length, void* arg) {
    WebSocketService* self = reinterpret_cast<WebSocketService*>(arg);
    if (self) {
        self->onEvent(clientId, type, payload, length);
    }
}

void WebSocketService::onEvent(uint8_t clientId, WStype_t type, uint8_t* payload, size_t length) {
    switch (type) {
        case WStype_CONNECTED:
            if (connectedClients >= 3) {
                Logger::log("[WebSocket] Cliente %u rechazado, máximo alcanzado\n", clientId);
                sendToClient(clientId, "Numero maximo de clientes alcanzado, intente mas tarde.");
                wsServer.disconnect(clientId);
                return;
            }
            connectedClients++;
            Logger::log("[WebSocket] Cliente %u conectado\n", clientId);
            break;

        case WStype_DISCONNECTED:
            connectedClients--;
            if (connectedClients < 0) connectedClients = 0;
            if (connectedClients == 0){
                
            }
            Logger::log("[WebSocket] Cliente %u desconectado\n", clientId);
            break;

        case WStype_TEXT: {
            String msg;
            for (size_t i = 0; i < length; i++) {
                msg += static_cast<char>(payload[i]);
            }
            Logger::log("[WebSocket] Mensaje recibido de [%u]: %s\n", clientId, msg.c_str());

            if (msg.startsWith("ping")) {
                sendToClient(clientId, "pong");
            } else {
                String response = cmdManager->processCommand(msg);
                sendToClient(clientId, response);
            }
            break;
        }

        case WStype_BIN:
            Logger::log("[WebSocket] Mensaje binario de [%u], longitud: %u\n", clientId, length);
            break;

        case WStype_ERROR:
            Logger::log("[WebSocket] Error de conexión con [%u]\n", clientId);
            break;

        case WStype_PING:
            Logger::log("[WebSocket] PING recibido de [%u]\n", clientId);
            break;

        case WStype_PONG:
            Logger::log("[WebSocket] PONG recibido de [%u]\n", clientId);
            break;

        default:
            Logger::log("[WebSocket] Evento desconocido: %u\n", type);
            break;
    }
}
