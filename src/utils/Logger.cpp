#include "Logger.h"
#include <stdarg.h>

bool Logger::enabled = false;

void Logger::enable(bool state) {
    enabled = state;
}

void Logger::log(const char* format, ...) {
    if (!enabled) return;

    va_list args;
    va_start(args, format);
    Serial.vprintf(format, args);
    va_end(args);
}
