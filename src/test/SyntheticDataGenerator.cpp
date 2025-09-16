#include "SyntheticDataGenerator.h"
#include <math.h>

SyntheticDataGenerator::SyntheticDataGenerator() : t(0.0) {}

int SyntheticDataGenerator::generateECG() {
    double ecg = sin(2 * M_PI * 1 * t) + 0.5 * sin(2 * M_PI * 60 * t) + 0.2 * nextGaussian();
    int ecgVal = (int) round(ecg * 1000 + 2048);
    t += DT;
    return clamp(ecgVal, 0, 0xFFFF);
}

PulseValues SyntheticDataGenerator::generatePulse() {
    PulseValues pulse;
    double irSignal = 2048 + 200 * sin(2 * M_PI * 1.2 * t) + 0.1 * nextGaussian();
    double redSignal = 2048 + 180 * cos(2 * M_PI * 0.8 * t) + 0.1 * nextGaussian();
    pulse.ir = clamp((int) round(irSignal), 0, 0xFFFF);
    pulse.red = clamp((int) round(redSignal), 0, 0xFFFF);
    return pulse;
}


double SyntheticDataGenerator::nextGaussian() {
    double u1 = random(1, 10000) / 10000.0;
    double u2 = random(1, 10000) / 10000.0;
    return sqrt(-2.0 * log(u1)) * cos(2 * M_PI * u2);
}

int SyntheticDataGenerator::clamp(int val, int min, int max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
}
