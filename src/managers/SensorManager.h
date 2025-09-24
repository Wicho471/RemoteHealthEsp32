#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include <Adafruit_MLX90614.h>
#include <MAX30105.h>
//#include <SparkFun_MMA8452Q.h> // Cambiar por un ADXL345
#include <Adafruit_Sensor.h>         // <-- CAMBIO: Librería base de Adafruit
#include <Adafruit_ADXL345_U.h>      // <-- CAMBIO: Librería para el ADXL345    
#include <Wire.h>

#include "PreferencesManager.h"
#include "src/config/Keys.h"
#include "src/config/PinConfig.h"
#include "src/utils/Logger.h"
#include "src/models/PulseValues.h"


class SensorManager {
public:
    SensorManager(PreferencesManager& prefs);

    void init();
    
    bool isMLXEnabled() const;
    bool isMAXEnabled() const;
    bool isACCELEnabled() const;
    bool isEcgConnected() const;
    
    void turnOnMax();
    void turnOffMax();

    float readTemperature();
    float readMovement();
    PulseValues readIRandRED();
    int readECG();

    void setMAX3010xBrightness(int brightness);
private:
    PreferencesManager& prefs;

    Adafruit_MLX90614 mlx;
    MAX30105 max3010x;
    //MMA8452Q accel;
    Adafruit_ADXL345_Unified accel; // <-- CAMBIO: Se reemplaza el objeto MMA8452Q por el ADXL345

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
