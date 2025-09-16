#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H

#include <Arduino.h>
#include "src/managers/SensorManager.h"
#include "src/models/SensorData.h"
#include "src/models/PulseValues.h"
#include "src/services/WebSocketService.h"
#include "src/utils/Logger.h"

/**
 * @brief Contexto global de la aplicación. Contiene los manejadores, servicios y variables compartidas.
 */
class AppContext {
public:
    explicit AppContext(SensorManager& sm, WebSocketService& ws);

    // Managers & Services
    SensorManager& sensorMgr;
    WebSocketService& wsService;

    portMUX_TYPE muxSensors;
    SemaphoreHandle_t i2cMutex;

    // Valores de sensores y flags
    int ecgValue;
    PulseValues pulse;
    float objectTemp;
    float movDelta;

    volatile bool newECGAvailable;
    volatile bool newTempAvailable;
    volatile bool newMovAvailable;
    volatile bool newPulseAvailable;

    void resetFlags();
    void resetValues();
    void copyValues(SensorData& data);
};

#endif 
