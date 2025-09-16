#ifndef WEBSOCKET_TASK_H
#define WEBSOCKET_TASK_H

#include <Arduino.h>
#include "src/services/WebSocketService.h"

/**
 * @brief Tarea encargada de despachar mensajes de la cola de WebSocketService.
 */
class WebSocketTask {
public:
  explicit WebSocketTask(WebSocketService& wsSrv);
  void start();

private:
  static void run(void* parameter);
  WebSocketService& wsSrv;
};

#endif // WEBSOCKET_TASK_H
