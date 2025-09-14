#include "SensorManager.h"
#include "src/config/PinConfig.h"

volatile bool SensorManager::loPlusDisconnected = false;
volatile bool SensorManager::loMinusDisconnected = false;

SensorManager::SensorManager(PreferencesManager& prefs) 
    : prefs(prefs), mlxOK(false), maxOK(false), accelOK(false) {
    i2cMutex = xSemaphoreCreateMutex();
    mlx = Adafruit_MLX90614();
}

void SensorManager::init() {
    Wire.begin(SDA_PIN, SCL_PIN, 100000);
    Wire1.begin(SDA_1_PIN, SCL_1_PIN, 400000);

    mlxOK = mlx.begin();
    Logger::log(mlxOK ? "Encendiendo sensor MLX90614\n" : "Sensor MLX90614 no encontrado\n");
    maxOK = max3010x.begin(Wire1);
    Logger::log(maxOK ? "Encendiendo sensor MAX3010x\n" : "Sensor MAX3010x no encontrado\n");

    if (maxOK) {
        int brightness = prefs.load<int>(KEY_OXI_BRIGHTNESS, 0x4F);
        max3010x.setup(     
            (uint8_t)brightness,
            4,
            2,
            400,
            411,
            4096
        );
        max3010x.enableFIFORollover();
    }
    turnOffMax();

    accelOK = accel.begin();
    Logger::log(accelOK ? "Encendiendo sensor MMA8452Q\n" : "Sensor MMA8452Q no encontrado\n");

    pinMode(LO_PLUS_PIN, INPUT);
    pinMode(LO_MINUS_PIN, INPUT);

    attachInterrupt(digitalPinToInterrupt(LO_PLUS_PIN), SensorManager::isrLoPlus, CHANGE);
    attachInterrupt(digitalPinToInterrupt(LO_MINUS_PIN), SensorManager::isrLoMinus, CHANGE);
}

void SensorManager::turnOnMax() {
    if (!maxOK) {
        Logger::log("Encendiendo sensor MAX3010x\n");
        max3010x.wakeUp();
        maxOK = true;
    }
}

void SensorManager::turnOffMax() {
    if (maxOK) {
        Logger::log("Apagando sensor MAX3010x\n");
        max3010x.shutDown();
        maxOK = false;
    }
}

bool SensorManager::isMLXReady() const { return mlxOK; }
bool SensorManager::isMAXReady() const { return maxOK; }
bool SensorManager::isACCELReady() const { return accelOK; }

bool SensorManager::isEcgConnected() const {
    return !(loPlusDisconnected || loMinusDisconnected);
}

float SensorManager::readTemperature() {
    if (!mlxOK) return NAN;
    if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        float temp = mlx.readObjectTempC();
        xSemaphoreGive(i2cMutex);
        return temp;
    }
    return NAN;
}

float SensorManager::readMovement() {
    if (!accelOK) return NAN;
    if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        float value = getAbsMoving();
        xSemaphoreGive(i2cMutex);
        return value;
    }
    return NAN;
}

long SensorManager::readIR() {
    if (!maxOK) return -1;
    if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        long value = max3010x.check() ? max3010x.getFIFOIR() : -1;
        xSemaphoreGive(i2cMutex);
        return value;
    }
    return -1;
}

long SensorManager::readRED() {
    if (!maxOK) return -1;
    if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        long value = max3010x.check() ? max3010x.getFIFORed() : -1;
        xSemaphoreGive(i2cMutex);
        return value;
    }
    return -1;
}

int SensorManager::readECG() {
    return isEcgConnected() ? analogRead(AD8232_PIN) : 0;
}

void SensorManager::setMAX3010xBrightness(uint8_t brightness) {
    if (!maxOK) return;
    brightness = constrain(brightness, 0, 255);
    prefs.save(KEY_OXI_BRIGHTNESS, brightness);
    max3010x.setPulseAmplitudeRed(brightness);
    max3010x.setPulseAmplitudeIR(brightness);
}

float SensorManager::getAbsMoving() {
    accel.read();
    float ax = accel.getCalculatedX();
    float ay = accel.getCalculatedY();
    float az = accel.getCalculatedZ();
    return ax * ax + ay * ay + az * az;
}

void IRAM_ATTR SensorManager::isrLoPlus() {
    loPlusDisconnected = digitalRead(LO_PLUS_PIN);
}

void IRAM_ATTR SensorManager::isrLoMinus() {
    loMinusDisconnected = digitalRead(LO_MINUS_PIN);
}

SemaphoreHandle_t SensorManager::getI2CMutex() {
    return i2cMutex;
}
