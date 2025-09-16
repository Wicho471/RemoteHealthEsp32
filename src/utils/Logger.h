#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

/**
 * @brief Logger general controlado por una sola bandera global.
 */
class Logger {
public:
    static void enable(bool state);
    static void setTest(bool state);
    static bool isTest();
    static void log(const char* format, ...);

private:
    static bool enabled;
    static bool testing;
};

#endif // LOGGER_H
