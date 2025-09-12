#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include <Adafruit_MLX90614.h>
#include <MAX30105.h>
#include <SparkFun_MMA8452Q.h>
#include <Wire.h>

#include "PreferencesManager.h"
#include "config/Keys.h"

class SensorManager {
public:
    SensorManager(PreferencesManager& prefs);

    void init();
    
    bool isMLXReady() const;
    bool isMAXReady() const;
    bool isACCELReady() const;
    bool isEcgConnected() const;

    float readTemperature();
    float readMovement();
    long readIR();
    long readRED();
    int readECG();

    void setMAX3010xBrightness(uint8_t brightness);

    SemaphoreHandle_t getI2CMutex(); // para tareas que necesiten sincronización

private:
    PreferencesManager& prefs;

    Adafruit_MLX90614 mlx;
    MAX30105 max3010x;
    MMA8452Q accel;

    SemaphoreHandle_t i2cMutex;

    bool mlxOK;
    bool maxOK;
    bool accelOK;

    float getAbsMoving(); // cálculo acelerómetro
};

#endif
