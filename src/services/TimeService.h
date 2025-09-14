#ifndef TIMESERVICE_H
#define TIMESERVICE_H

#include <Arduino.h>
#include <time.h>
#include "src/config/Keys.h"
#include "src/managers/PreferencesManager.h"

class TimeService {
public:
    explicit TimeService(PreferencesManager* prefsManager);
    bool setupTime();
    unsigned long long currentTimeMillis();

private:
    PreferencesManager* prefs;
    void loadLastTimeSaved();
    String getFormattedTime(struct tm& timeinfo);
};

#endif
