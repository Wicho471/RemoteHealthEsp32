#include "WebSocketService.h"

WebSocketService* WebSocketService::instance = nullptr;

WebSocketService::WebSocketService(uint16_t port, size_t qSize, size_t payloadMax)
  : wsServer(nullptr),
    wsPort(port),
    queueSize(qSize),
    payloadMaxSize(payloadMax),
    wsQueue(nullptr),
    senderTaskHandle(nullptr),
    onTextMessage(nullptr),
    onClientConnected(nullptr),
    onClientDisconnected(nullptr)
{
  // Mantener referencia singleton para callback C-style
  instance = this;
}

WebSocketService::~WebSocketService() {
  if (wsServer) {
    wsServer->close();
    delete wsServer;
    wsServer = nullptr;
  }
  if (wsQueue) {
    vQueueDelete(wsQueue);
    wsQueue = nullptr;
  }
  // Nota: No eliminamos la tarea senderTaskHandle aquí (si existe),
  // porque FreeRTOS puede requerir un manejo más cuidadoso en runtime.
}

/**
 * begin: crea servidor, cola y tarea sender.
 */
bool WebSocketService::begin() {
  // Crear servidor WebSocket
  wsServer = new WebSocketsServer(wsPort);
  if (!wsServer) return false;
  wsServer->begin();
  wsServer->onEvent(WebSocketService::_onWsEvent);

  // Crear cola
  wsQueue = xQueueCreate(static_cast<UBaseType_t>(queueSize), static_cast<UBaseType_t>(payloadMaxSize));
  if (wsQueue == NULL) {
    Serial.println("[WebSocketService] Error creando cola");
    return false;
  }

  // Crear tarea de envío
  BaseType_t res = xTaskCreatePinnedToCore(
    WebSocketService::TaskWebSocketSender,
    "WS_Sender",
    4096,
    this,
    2,
    &senderTaskHandle,
    1
  );

  if (res != pdPASS) {
    Serial.println("[WebSocketService] Error creando tarea WS Sender");
    vQueueDelete(wsQueue);
    wsQueue = nullptr;
    return false;
  }

  Serial.printf("[WebSocketService] Iniciado en puerto %u (cola=%u, payload=%u)\n", wsPort, (unsigned)queueSize, (unsigned)payloadMaxSize);
  return true;
}

void WebSocketService::loop() {
  if (wsServer) wsServer->loop();
}

bool WebSocketService::enqueuePayload(const char* payload) {
  if (!wsQueue || !payload) return false;

  size_t len = strnlen(payload, payloadMaxSize);
  if (len == 0) return false;

  char tmp[64];
  // Si payload > payloadMaxSize, truncamos. Usamos envío en trozos de payloadMaxSize.
  if (len >= payloadMaxSize) {
    // tomar últimos payloadMaxSize-1 bytes (o primeros). Elegimos primeros.
    // Copiar y asegurar null-termination.
    char* buf = (char*)malloc(payloadMaxSize);
    if (!buf) return false;
    memset(buf, 0, payloadMaxSize);
    memcpy(buf, payload, payloadMaxSize - 1);
    BaseType_t ok = xQueueSend(wsQueue, buf, 0);
    free(buf);
    return ok == pdTRUE;
  } else {
    // copia pequeña en stack si cabe
    // payloadMaxSize puede ser grande; proteger con límite razonable
    if (payloadMaxSize <= 1024) {
      char buf[1024];
      memset(buf, 0, payloadMaxSize);
      memcpy(buf, payload, len + 1);
      BaseType_t ok = xQueueSend(wsQueue, buf, 0);
      return ok == pdTRUE;
    } else {
      // payloadMaxSize demasiado grande para stack -> malloc
      char* buf = (char*)malloc(payloadMaxSize);
      if (!buf) return false;
      memset(buf, 0, payloadMaxSize);
      memcpy(buf, payload, len + 1);
      BaseType_t ok = xQueueSend(wsQueue, buf, 0);
      free(buf);
      return ok == pdTRUE;
    }
  }
}

void WebSocketService::broadcastNow(const char* payload) {
  if (!wsServer || !payload) return;
  wsServer->broadcastTXT(payload);
}

void WebSocketService::setOnTextMessageCallback(std::function<void(const String&)> cb) {
  onTextMessage = cb;
}

void WebSocketService::setOnClientConnectedCallback(std::function<void(uint8_t)> cb) {
  onClientConnected = cb;
}

void WebSocketService::setOnClientDisconnectedCallback(std::function<void(uint8_t)> cb) {
  onClientDisconnected = cb;
}

int WebSocketService::connectedClients() const {
  if (!wsServer) return 0;
  return wsServer->connectedClients();
}

void WebSocketService::resetQueue() {
  if (wsQueue) xQueueReset(wsQueue);
}

/* ------------------------------------------------------------------
   Callbacks / tareas estáticas
   ------------------------------------------------------------------ */

void WebSocketService::_onWsEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  if (instance) instance->handleWsEvent(num, type, payload, length);
}

void WebSocketService::handleWsEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED: {
      IPAddress ip = wsServer->remoteIP(num);
      Serial.printf("[WebSocketService] Cliente conectado [%u] - %s\n", num, ip.toString().c_str());
      if (onClientConnected) onClientConnected(num);
      break;
    }

    case WStype_DISCONNECTED: {
      Serial.printf("[WebSocketService] Cliente desconectado [%u]\n", num);
      if (connectedClients() == 0) {
        // Si ya no quedan clientes, reseteamos cola para liberar memoria/cambios de estado
        resetQueue();
      }
      if (onClientDisconnected) onClientDisconnected(num);
      break;
    }

    case WStype_TEXT: {
      // Payload no está garantizado terminada en '\0', así que construimos String.
      String msg;
      msg.reserve(length + 1);
      for (size_t i = 0; i < length; ++i) msg += (char)payload[i];

      Serial.printf("[WebSocketService] Texto de [%u]: %s\n", num, msg.c_str());
      if (onTextMessage) onTextMessage(msg);
      break;
    }

    case WStype_BIN: {
      Serial.printf("[WebSocketService] Mensaje binario [%u], len=%u\n", num, (unsigned)length);
      break;
    }

    case WStype_PING:
      Serial.printf("[WebSocketService] PING de [%u]\n", num);
      break;

    case WStype_PONG:
      Serial.printf("[WebSocketService] PONG de [%u]\n", num);
      break;

    case WStype_ERROR:
      Serial.printf("[WebSocketService] ERROR con [%u]\n", num);
      break;

    default:
      Serial.printf("[WebSocketService] Evento desconocido (%u) de [%u]\n", (unsigned)type, num);
      break;
  }
}

/**
 * Task: toma mensajes de la cola y los transmite a todos los clientes.
 * Implementa comportamiento similar al código original TaskWebSocketSender.
 */
void WebSocketService::TaskWebSocketSender(void* parameter) {
  WebSocketService* self = static_cast<WebSocketService*>(parameter);
  if (!self) {
    vTaskDelete(NULL);
    return;
  }

  // Buffer temporal para recibir elementos de la cola. Reservamos payloadMaxSize.
  size_t bufSize = self->payloadMaxSize;
  char* recvBuf = (char*)malloc(bufSize);
  if (!recvBuf) {
    Serial.println("[WebSocketService] No se pudo asignar buffer para sender task");
    vTaskDelete(NULL);
    return;
  }

  while (true) {
    if (xQueueReceive(self->wsQueue, recvBuf, portMAX_DELAY) == pdTRUE) {
      if (self->connectedClients() > 0) {
        // Asegurar terminación nula
        recvBuf[bufSize - 1] = '\0';
        self->wsServer->broadcastTXT(recvBuf);
      }
      // pequeña pausa cooperativa
      vTaskDelay(pdMS_TO_TICKS(1));
      // limpiar buffer para la próxima iteración
      memset(recvBuf, 0, bufSize);
    }
  }

  // nunca llega aquí
  free(recvBuf);
  vTaskDelete(NULL);
}

QueueHandle_t WebSocketService::getQueueHandle() const {
    return wsQueue;
}
