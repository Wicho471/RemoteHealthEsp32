#include "SensorData.h"

SensorData::SensorData(TimeService *timeService) : timeService(timeService) {
  reset();
}

void SensorData::toPayload(char *buffer, size_t bufferSize) const {
  if (Logger::isTest()) {
    // Generador sintético
    static SyntheticDataGenerator generator;
    PulseValues syntheticPulse = generator.generatePulse();

    int len = 0;
    len += snprintf(buffer + len, bufferSize - len, "%llX,", timeService->currentTimeMillis());

    len += snprintf(buffer + len, bufferSize - len, "%X,", generator.generateECG());
    len += snprintf(buffer + len, bufferSize - len, "%.2f,", 0.99);
    len += snprintf(buffer + len, bufferSize - len, "%.2f,", 36.5);
    len += snprintf(buffer + len, bufferSize - len, "%lX,", syntheticPulse.ir);
    len += snprintf(buffer + len, bufferSize - len, "%lX", syntheticPulse.red);
    buffer[len] = '\0';
    return;
  }

  int len = 0;
  len += snprintf(buffer + len, bufferSize - len, "%llX,", timeService->currentTimeMillis());

  if (ecgReady) len += snprintf(buffer + len, bufferSize - len, "%X,", ecg);
  else len += snprintf(buffer + len, bufferSize - len, "%s,", NR);

  if (movReady) len += snprintf(buffer + len, bufferSize - len, "%.2f,", movement);
  else if (accelOK) len += snprintf(buffer + len, bufferSize - len, "%s,", NR);
  else len += snprintf(buffer + len, bufferSize - len, "%s,", ERR);

  if (tempReady) len += snprintf(buffer + len, bufferSize - len, "%.2f,", temperature);
  else if (mlxOK) len += snprintf(buffer + len, bufferSize - len, "%s,", NR);
  else len += snprintf(buffer + len, bufferSize - len, "%s,", ERR);

  if (pulsesReady) len += snprintf(buffer + len, bufferSize - len, "%lX,", pulse.ir);
  else if (maxOK) len += snprintf(buffer + len, bufferSize - len, "%s,", NR);
  else len += snprintf(buffer + len, bufferSize - len, "%s,", ERR);

  if (pulsesReady) len += snprintf(buffer + len, bufferSize - len, "%lX", pulse.red);
  else if (maxOK) len += snprintf(buffer + len, bufferSize - len, "%s", NR);
  else len += snprintf(buffer + len, bufferSize - len, "%s", ERR);
  buffer[len] = '\0';
}

void SensorData::reset() {
    ecg = -1;
    movement = NAN;
    temperature = NAN;
    pulse.ir = -1;
    pulse.red = -1;

    ecgReady = false;
    movReady = false;
    tempReady = false;
    pulsesReady = false;

    accelOK = false;
    mlxOK = false;
    maxOK = false;
}