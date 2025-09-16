#include "AppContext.h"

AppContext::AppContext(SensorManager& sm, WebSocketService& ws)
    : sensorMgr(sm),
      wsService(ws),
      muxSensors(portMUX_INITIALIZER_UNLOCKED),
      i2cMutex(xSemaphoreCreateMutex()),
      ecgValue(0),
      pulse(INVALID_PULSE_VALUES),
      objectTemp(0.0f),
      movDelta(0.0f),
      newECGAvailable(false),
      newPulseAvailable(false),
      newTempAvailable(false),
      newMovAvailable(false)
      {}

void AppContext::resetFlags() {
    newECGAvailable = false;
    newPulseAvailable = false;
    newTempAvailable = false;
    newMovAvailable = false;
}

void AppContext::resetValues() {
    ecgValue = -1;
    pulse = INVALID_PULSE_VALUES;
    objectTemp = NAN;
    movDelta = NAN;
}

void AppContext::copyValues(SensorData& data) {
    data.ecg = ecgValue;
    data.temperature = objectTemp;
    data.movement = movDelta;
    data.pulse = pulse;

    data.ecgReady = newECGAvailable;
    data.tempReady = newTempAvailable;
    data.movReady = newMovAvailable;
    data.pulsesReady = newPulseAvailable;
}