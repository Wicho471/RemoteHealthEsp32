#ifndef WEBSOCKET_SERVICE_H
#define WEBSOCKET_SERVICE_H

#include <WebSocketsServer.h>
#include <Arduino.h>

/**
 * @brief Servicio que encapsula la lógica del servidor WebSocket,
 *        la cola de mensajes y la tarea que despacha mensajes a clientes.
 *
 * Diseñado para integrarse con los managers existentes mediante callbacks.
 */
class WebSocketService {
public:
  /**
   * @brief Constructor.
   * @param port Puerto WebSocket (por defecto 8080).
   * @param queueSize Cantidad máxima de elementos en la cola (por defecto 10).
   * @param payloadMaxSize Tamaño máximo de cada payload en bytes (por defecto 512).
   */
  WebSocketService(uint16_t port = 8080, size_t queueSize = 10, size_t payloadMaxSize = 512);

  ~WebSocketService();

  /**
   * @brief Inicia el servicio (crea servidor, cola y tarea sender).
   *        Debe llamarse desde setup().
   * @return true si la inicialización fue satisfactoria.
   */
  bool begin();

  /**
   * @brief Debe llamarse desde loop() del sketch para procesar el WebSocket.
   */
  void loop();

  /**
   * @brief Coloca un payload (C-string) en la cola para ser enviado por la tarea
   *        de envío. Hace copia interna limitada a payloadMaxSize.
   * @param payload Cadena terminada en '\0'.
   * @return true si se encoló correctamente, false si la cola estaba llena o error.
   */
  bool enqueuePayload(const char* payload);

  /**
   * @brief Transmite inmediatamente a todos los clientes (no usa cola).
   * @param payload C-string.
   */
  void broadcastNow(const char* payload);

  /**
   * @brief Registra callback para mensajes de texto entrantes.
   * @param cb función que recibe el payload como String.
   */
  void setOnTextMessageCallback(std::function<void(const String&)> cb);

  /**
   * @brief Registra callback para eventos de conexión (cliente conectado).
   */
  void setOnClientConnectedCallback(std::function<void(uint8_t)> cb);

  /**
   * @brief Registra callback para eventos de desconexión (cliente desconectado).
   */
  void setOnClientDisconnectedCallback(std::function<void(uint8_t)> cb);

  /**
   * @brief Obtiene el número actual de clientes conectados.
   */
  int connectedClients() const;

  /**
   * @brief Resetea la cola de mensajes.
   */
  void resetQueue();

  QueueHandle_t getQueueHandle() const;

private:
  WebSocketsServer* wsServer;
  uint16_t wsPort;
  size_t queueSize;
  size_t payloadMaxSize;
  QueueHandle_t wsQueue;
  TaskHandle_t senderTaskHandle;

  static WebSocketService* instance; ///< Instancia singleton para callbacks C-style

  // Callbacks del usuario
  std::function<void(const String&)> onTextMessage;
  std::function<void(uint8_t)> onClientConnected;
  std::function<void(uint8_t)> onClientDisconnected;

  // Handler estático requerido por la librería (encamina a miembro)
  static void _onWsEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length);

  // Método no-estático que procesa el evento real
  void handleWsEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length);

  // Tarea que saca elementos de la cola y los transmite
  static void TaskWebSocketSender(void* parameter);

  // No permitir copia
  WebSocketService(const WebSocketService&) = delete;
  WebSocketService& operator=(const WebSocketService&) = delete;
};

#endif // WEBSOCKET_SERVICE_H
