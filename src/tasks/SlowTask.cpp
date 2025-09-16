#include "SlowTask.h"

SlowTask::SlowTask(AppContext& ctx)
  : context(ctx) {}

void SlowTask::start() {
  xTaskCreatePinnedToCore(
    SlowTask::run,
    "SlowTask",
    4096,
    this,
    1,
    nullptr,
    1
  );
}

void SlowTask::run(void* parameter) {
  auto* self = static_cast<SlowTask*>(parameter);
  uint32_t lastTemp = 0;

  while (true) {
    if (self->context.wsService.getConnectedClients() == 0) {
      vTaskDelay(pdMS_TO_TICKS(100));
      continue;
    }

    if (self->context.sensorMgr.isMLXEnabled() && millis() - lastTemp > 500) {
      xSemaphoreTake(self->context.i2cMutex, portMAX_DELAY);
      float temp = self->context.sensorMgr.readTemperature();
      xSemaphoreGive(self->context.i2cMutex);

      portENTER_CRITICAL(&self->context.muxSensors);
      self->context.objectTemp = temp;
      self->context.newTempAvailable = true;
      lastTemp = millis();
      portEXIT_CRITICAL(&self->context.muxSensors);
    }

    if (self->context.sensorMgr.isACCELEnabled()) {
      xSemaphoreTake(self->context.i2cMutex, portMAX_DELAY);
      float movement = self->context.sensorMgr.readMovement();
      xSemaphoreGive(self->context.i2cMutex);

      portENTER_CRITICAL(&self->context.muxSensors);
      self->context.movDelta = movement;
      self->context.newMovAvailable = true;
      portEXIT_CRITICAL(&self->context.muxSensors);
    }

    vTaskDelay(pdMS_TO_TICKS(250)); // 250 ms entre ciclos
  }
}
