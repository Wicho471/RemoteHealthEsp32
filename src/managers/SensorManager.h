#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include <Adafruit_MLX90614.h>
#include <MAX30105.h>
#include <SparkFun_MMA8452Q.h>
#include <Wire.h>

#include "PreferencesManager.h"
#include "src/config/Keys.h"
#include "src/utils/Logger.h"


class SensorManager {
public:
    SensorManager(PreferencesManager& prefs);

    void init();
    
    bool isMLXReady() const;
    bool isMAXReady() const;
    bool isACCELReady() const;
    bool isEcgConnected() const;

    void turnOnMax();
    void turnOffMax();

    float readTemperature();
    float readMovement();
    long readIR();
    long readRED();
    int readECG();

    void setMAX3010xBrightness(uint8_t brightness);

    SemaphoreHandle_t getI2CMutex();

private:
    PreferencesManager& prefs;

    Adafruit_MLX90614 mlx;
    MAX30105 max3010x;
    MMA8452Q accel;

    SemaphoreHandle_t i2cMutex;

    bool mlxOK;
    bool maxOK;
    bool accelOK;

    static volatile bool loPlusDisconnected;
    static volatile bool loMinusDisconnected;

    static void IRAM_ATTR isrLoPlus();
    static void IRAM_ATTR isrLoMinus();

    float getAbsMoving(); 
};

#endif
