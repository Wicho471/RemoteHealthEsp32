#ifndef PULSE_VALUES_H
#define PULSE_VALUES_H

/**
 * Estructura que contiene los valores del sensor de pulso:
 * IR y Red obtenidos en un único chequeo.
 */
struct PulseValues {
    long ir;
    long red;

    /**
     * Operador de igualdad para comparar dos PulseValues rápidamente.
     */
    bool operator==(const PulseValues& other) const {
        return ir == other.ir && red == other.red;
    }

    bool operator!=(const PulseValues& other) const {
        return !(*this == other);
    }
};

/**
 * Constante que representa un valor inválido de pulso (sin datos).
 */
inline const PulseValues INVALID_PULSE_VALUES = { -1, -1 };

#endif
