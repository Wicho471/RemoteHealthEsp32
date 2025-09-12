#include "PreferencesManager.h"
#include "NetworkManager.h"
#include "SensorManager.h"
#include "CommandManager.h"

PreferencesManager prefs(KEY_NAMESPACE);
WifiManager wifi(&prefs);
SensorManager sensors(prefs);
CommandManager cmd(&wifi, prefs, &sensors);

void setup() {
  Serial.begin(115200);
  wifi.begin();
  sensors.init();
}

void loop(){
  delay(100000);
}