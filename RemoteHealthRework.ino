#include "src/utils/Logger.h"

#include "src/managers/PreferencesManager.h"
#include "src/managers/NetworkManager.h"
#include "src/managers/SensorManager.h"
#include "src/managers/CommandManager.h"
#include "src/managers/SerialManager.h"
#include "src/services/TimeService.h"
#include "src/services/WebServerService.h"
//#include "src/tasks/FastTask.h"
//#include "src/tasks/SlowTask.h"
//#include "src/tasks/WebSocketTask.h"
//#include "src/services/WebSocketService.h"

PreferencesManager prefs(KEY_NAMESPACE);
TimeService timeSrv(&prefs);
WifiManager wifi(&prefs, &timeSrv);
SensorManager sensorMgr(prefs);
CommandManager cmd(&wifi, prefs, &sensorMgr);
SerialManager serialMgr(&cmd);
WebServerService webSrv(&wifi);
//WebSocketService wsSrv(8080, 10, 512);

//FastTask fastTask(sensorMgr, wsSrv);
//SlowTask slowTask(sensorMgr, wsSrv);
//WebSocketTask wsTask(wsSrv);

void setup() {
  serialMgr.begin();
  Logger::enable(true);

  wifi.begin();
  sensorMgr.init();
  webSrv.begin();
  //wsSrv.begin();

  //fastTask.start();
  //slowTask.start();
  //wsTask.start();
}

void loop(){
  delay(1);
  serialMgr.handleSerial();
  webSrv.loop();
}