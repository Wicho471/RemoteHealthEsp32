#ifndef SYNTHETIC_DATA_GENERATOR_H
#define SYNTHETIC_DATA_GENERATOR_H

#include <Arduino.h>
#include "src/models/PulseValues.h"

/**
 * @brief Generador de datos sintéticos para pruebas y depuración.
 * Genera señales de ECG y pulsos IR/Red, el resto de valores son constantes.
 */
class SyntheticDataGenerator {
public:
    SyntheticDataGenerator();

    /**
     * @brief Genera un ECG sintético.
     * @return Valor del ECG escalado a 0..4095 (12 bits).
     */
    int generateECG();

    /**
     * @brief Genera valores sintéticos de pulso IR y Red.
     * @param ir Referencia donde guardar el valor IR.
     * @param red Referencia donde guardar el valor Red.
     */
    PulseValues generatePulse();

private:
    double t;  
    static constexpr double DT = 0.003;

    double nextGaussian();
    int clamp(int val, int min, int max);
};

#endif
