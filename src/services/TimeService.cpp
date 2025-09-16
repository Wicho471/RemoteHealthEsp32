#include "TimeService.h"
#include "src/utils/Logger.h"

const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const char* ntpServer3 = "time.google.com";
const long gmtOffset_sec = -6 * 3600;
const int daylightOffset_sec = 0;

TimeService::TimeService(PreferencesManager* prefsManager)
    : prefs(prefsManager) {}

bool TimeService::setupTime() {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2, ntpServer3);
    struct tm timeinfo;

    Logger::log("Esperando sincronización NTP");
    for (int i = 0; i < 10; i++) {
        Logger::log(".");
        if (getLocalTime(&timeinfo)) {
            time_t now;
            time(&now);
            prefs->save(KEY_TIME, static_cast<uint64_t>(now));
            Logger::log("\nHora establecida correctamente -> %s\n", getFormattedTime(timeinfo).c_str());
            return true;
        }
        delay(100);
    }
    Logger::log("No se pudo establecer la hora.");
    loadLastTimeSaved();
    return false;
}

unsigned long long TimeService::currentTimeMillis() {
  struct timeval tv;
  gettimeofday(&tv, nullptr);
  unsigned long long millisSinceEpoch = (static_cast<unsigned long long>(tv.tv_sec) * 1000ULL) + (static_cast<unsigned long long>(tv.tv_usec) / 1000ULL);
  return millisSinceEpoch;
}

void TimeService::loadLastTimeSaved() {
    Logger::log("Cargando ultima hora guardada\n");
    uint64_t saved = prefs->load<uint64_t>(KEY_TIME, 0);
    if (saved > 0) {
        struct timeval tv = { .tv_sec = static_cast<time_t>(saved), .tv_usec = 0 };
        settimeofday(&tv, nullptr);
    }
}

String TimeService::getFormattedTime(struct tm& timeinfo) {
    char buffer[64]; 
    strftime(buffer, sizeof(buffer), "%A, %d %B %Y %H:%M:%S", &timeinfo);
    String formattedTime = String(buffer);
    return formattedTime;
}
