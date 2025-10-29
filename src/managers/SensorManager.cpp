#include "SensorManager.h"

volatile bool SensorManager::loPlusDisconnected = false;
volatile bool SensorManager::loMinusDisconnected = false;

// <-- CAMBIO: Se inicializa el objeto ADXL345 con un ID de sensor único
SensorManager::SensorManager(PreferencesManager& prefs) 
    : prefs(prefs), accel(12345), mlxOK(false), maxOK(false), accelOK(false) {
    mlx = Adafruit_MLX90614();
}

void SensorManager::init() {
    Wire.begin(SDA_PIN, SCL_PIN, 100000);
    Wire1.begin(SDA_1_PIN, SCL_1_PIN, 400000);

    pinMode(SDA_PIN, INPUT_PULLUP);
    pinMode(SCL_PIN, INPUT_PULLUP);
    pinMode(SDA_1_PIN, INPUT_PULLUP);
    pinMode(SCL_1_PIN, INPUT_PULLUP);

    Wire.setTimeout(10000); 
    Wire1.setTimeout(10000);

    // Bucle de inicialización para el sensor MLX90614
    int attempts = 0;
    while (!mlxOK && attempts < 10) {
        mlxOK = mlx.begin();
        if (!mlxOK) {
            Logger::log("Intentando conectar con sensor MLX90614...\n");
            delay(500);
        }
        attempts++;
    }
    Logger::log(mlxOK ? "Encendido sensor MLX90614\n" : "Sensor MLX90614 no encontrado despues de multiples intentos.\n");

    // Bucle de inicialización para el sensor MAX3010x
    attempts = 0;
    while (!maxOK && attempts < 10) {
        maxOK = max3010x.begin(Wire1);
        if (!maxOK) {
            Logger::log("Intentando conectar con sensor MAX3010x...\n");
            delay(500);
        }
        attempts++;
    }
    Logger::log(maxOK ? "Encendido sensor MAX3010x\n" : "Sensor MAX3010x no encontrado despues de multiples intentos.\n");

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

    // Bucle de inicialización para el sensor ADXL345
    attempts = 0;
    while (!accelOK && attempts < 10) {
        accelOK = accel.begin();
        if (!accelOK) {
            Logger::log("Intentando conectar con sensor ADXL345...\n");
            delay(500);
        }
        attempts++;
    }
    if (accelOK) {
        accel.setRange(ADXL345_RANGE_2_G);
    }
    Logger::log(accelOK ? "Encendido sensor ADXL345\n" : "Sensor ADXL345 no encontrado despues de multiples intentos.\n");

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

bool SensorManager::isMLXEnabled() const { return mlxOK; }
bool SensorManager::isMAXEnabled() const { return maxOK; }
bool SensorManager::isACCELEnabled() const { return accelOK; }

bool SensorManager::isEcgConnected() const {
    return !(loPlusDisconnected || loMinusDisconnected);
}

float SensorManager::readTemperature() {
    if (!mlxOK) return NAN;
    return mlx.readObjectTempC();
}

float SensorManager::readMovement() {
    if (!accelOK) return NAN;
    return getAbsMoving();
}

PulseValues SensorManager::readIRandRED() {
    PulseValues values = INVALID_PULSE_VALUES;
    if (!maxOK) return values;
    if (max3010x.check()) {
        values.ir = max3010x.getFIFOIR();
        values.red = max3010x.getFIFORed();
    }
    return values;
}

int SensorManager::readECG() {
    return isEcgConnected() ? analogRead(AD8232_PIN) : 0;
}

void SensorManager::setMAX3010xBrightness(int brightness) {
    Logger::log("Guardado brillo ->");
    brightness = constrain(brightness, 0, 255);
    if( prefs.save(KEY_OXI_BRIGHTNESS, brightness)){
        Logger::log("OK\n");
    } else {
        Logger::log("ERROR\n");
    }
    if (!maxOK) return;
    max3010x.setPulseAmplitudeRed(brightness);
    max3010x.setPulseAmplitudeIR(brightness);
}

// <-- CAMBIO: Lectura de datos usando el modelo de "eventos" de Adafruit
float SensorManager::getAbsMoving() {
    sensors_event_t event; 
    accel.getEvent(&event);
    
    float ax = event.acceleration.x;
    float ay = event.acceleration.y;
    float az = event.acceleration.z;
    
    // Calcula el cuadrado de la magnitud del vector de aceleración
    return ax * ax + ay * ay + az * az;
}

void IRAM_ATTR SensorManager::isrLoPlus() {
    loPlusDisconnected = digitalRead(LO_PLUS_PIN);
}

void IRAM_ATTR SensorManager::isrLoMinus() {
    loMinusDisconnected = digitalRead(LO_MINUS_PIN);
}
