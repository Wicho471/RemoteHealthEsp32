#include "Logger.h"
#include <stdarg.h>

bool Logger::enabled = false;
bool Logger::testing = false;

void Logger::enable(bool state) {
    enabled = state;
}

void Logger::setTest(bool state) {
    testing = state;
}

bool Logger::isTest() {
    return testing;
}

void Logger::log(const char* format, ...) {
    if (!enabled) return;

    va_list args;
    va_start(args, format);
    Serial.vprintf(format, args);
    va_end(args);
}
