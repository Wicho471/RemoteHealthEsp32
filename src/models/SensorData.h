#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include <Arduino.h>
#include "PulseValues.h"
#include "src/services/TimeService.h"
#include "src/utils/Logger.h"
#include "src/test/SyntheticDataGenerator.h"

/**
 * Representa los datos capturados por los sensores.
 */
struct SensorData {
public:
    explicit SensorData(TimeService* timeService);

    unsigned long long timestamp;
    int ecg;
    float movement;
    float temperature;
    PulseValues pulse;

    bool ecgReady;
    bool movReady;
    bool tempReady;
    bool pulsesReady;

    bool accelOK;
    bool mlxOK;
    bool maxOK;

    void toPayload(char* buffer, size_t bufferSize) const;
    void reset();

private: 
    TimeService* timeService;
    inline static constexpr const char* ERR = "ERR";
    inline static constexpr const char* NR  = "NR";
};

#endif // SENSOR_DATA_H
