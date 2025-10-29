#include "WebSocketTask.h"

WebSocketTask::WebSocketTask(WebSocketService& ws)
  : wsSrv(ws) {}

void WebSocketTask::start() {
  xTaskCreatePinnedToCore(
    WebSocketTask::run,
    "WebSocketTask",
    4096,
    this,
    2,
    nullptr,
    1
  );
}

void WebSocketTask::run(void* parameter) {
  auto* self = static_cast<WebSocketTask*>(parameter);
  char msg[512]; // coincide con payloadMaxSize

  while (true) {
    if (xQueueReceive(self->wsSrv.getQueueHandle(), msg, portMAX_DELAY) == pdTRUE) {
      if (self->wsSrv.getConnectedClients() > 0) {
        self->wsSrv.broadcast(msg);
      }
    }
    vTaskDelay(pdMS_TO_TICKS(2.5));
  }
}
