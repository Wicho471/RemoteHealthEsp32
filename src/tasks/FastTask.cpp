#include "FastTask.h"

FastTask::FastTask(AppContext& ctx) : context(ctx) {}

void FastTask::start() {
  xTaskCreatePinnedToCore(
    FastTask::run,
    "SensorReader_Fast",
    8192,
    this,
    2,
    nullptr,
    1
  );
}

void FastTask::run(void* parameter) {
  auto* self = static_cast<FastTask*>(parameter);

  while (true) {
    if (self->context.wsService.getConnectedClients() == 0) {
      vTaskDelay(pdMS_TO_TICKS(100));
      continue;
    }

    portENTER_CRITICAL(&self->context.muxSensors);
    self->context.ecgValue = self->context.sensorMgr.readECG();
    self->context.newECGAvailable = true;
    portEXIT_CRITICAL(&self->context.muxSensors);

    if (self->context.sensorMgr.isMAXEnabled()) {
      if (xSemaphoreTake(self->context.i2cMutex, portMAX_DELAY) == pdTRUE) {
        PulseValues pulseValues = self->context.sensorMgr.readIRandRED();
        if (pulseValues != INVALID_PULSE_VALUES) {
          xSemaphoreGive(self->context.i2cMutex);

          portENTER_CRITICAL(&self->context.muxSensors);
          self->context.pulse = pulseValues;
          self->context.newPulseAvailable = true;
          portEXIT_CRITICAL(&self->context.muxSensors);
        } else {
          xSemaphoreGive(self->context.i2cMutex);
        }
      }
    }

    vTaskDelay(pdMS_TO_TICKS(4));
  }
}
