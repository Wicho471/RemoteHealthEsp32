#include "WebSocketService.h"

WebSocketService::WebSocketService(CommandManager* cmdManager, uint16_t port) :
    _port(port),
    _cmdManager(cmdManager),
    _isInitialized(false),
    _server(nullptr),
    _ws(nullptr),
    _queueHandle(nullptr)
{}

WebSocketService::~WebSocketService() {
    delete _ws;
    delete _server;
}

bool WebSocketService::begin() {
    if (!_certManager.initialize()) {
        return false;
    }

    _server = new AsyncWebServer(_port, true, _certManager.getCert().c_str(), _certManager.getKey().c_str());
    if(!_server) {
        return false;
    }
    
    _ws = new AsyncWebSocket("/ws");
    if(!_ws) {
        return false;
    }

    _ws->onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
        this->onEvent(server, client, type, arg, data, len);
    });

    _server->addHandler(_ws);
    
    _queueHandle = xQueueCreate(QUEUE_SIZE, sizeof(WebSocketEventData));
    if (_queueHandle == nullptr) {
        return false;
    }

    _server->begin();
    
    _isInitialized = true;
    return true;
}

void WebSocketService::broadcast(const String& message) {
    if (!_isInitialized) return;
    _ws->textAll(message);
}

void WebSocketService::sendToClient(uint32_t clientId, const String& message) {
    if (!_isInitialized) return;
    _ws->text(clientId, message);
}

size_t WebSocketService::getConnectedClients() const {
    if (!_isInitialized) return 0;
    return _ws->count();
}

QueueHandle_t WebSocketService::getQueueHandle() const {
    return _queueHandle;
}

void WebSocketService::onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    switch (type) {
        case WS_EVT_CONNECT: {
            if (_ws->count() >= MAX_WS_CLIENTS) {
                client->close();
                return;
            }
            break;
        }
        case WS_EVT_DISCONNECT:
            break;
        case WS_EVT_DATA: {
            AwsFrameInfo *info = (AwsFrameInfo*)arg;
            if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
                WebSocketEventData eventData;
                eventData.clientId = client->id();

                strncpy(eventData.payload, (const char*)data, PAYLOAD_MAX_SIZE);
                eventData.payload[PAYLOAD_MAX_SIZE - 1] = '\0';

                xQueueSend(_queueHandle, &eventData, pdMS_TO_TICKS(10));
            }
            break;
        }
        case WS_EVT_ERROR:
            break;
        default:
            break;
    }
}