#include "src/config/Constans.h"
#include "src/core/AppContext.h"
#include "src/managers/CommandManager.h"
#include "src/managers/NetworkManager.h"
#include "src/managers/PreferencesManager.h"
#include "src/managers/SensorManager.h"
#include "src/managers/SerialManager.h"
#include "src/models/SensorData.h"
#include "src/services/TimeService.h"
#include "src/services/WebServerService.h"
#include "src/services/WebSocketService.h"
#include "src/tasks/FastTask.h"
#include "src/tasks/SlowTask.h"
#include "src/tasks/WebSocketTask.h"
#include "src/utils/Logger.h"

PreferencesManager prefs(KEY_NAMESPACE);
TimeService timeSrv(&prefs);
WifiManager wifi(&prefs, &timeSrv);
SensorManager sensorMgr(prefs);
CommandManager cmd(&wifi, prefs, &sensorMgr);
SerialManager serialMgr(&cmd);
WebServerService webSrv(&wifi);
WebSocketService wsSrv(&cmd);

AppContext appCtx(sensorMgr, wsSrv);
FastTask fastTask(appCtx);
SlowTask slowTask(appCtx);
WebSocketTask wsTask(wsSrv);

SensorData sensorData(&timeSrv);

char buffer[BUFFER_SIZE];

void setup() {
  serialMgr.begin();  //Cambiar el nombre al metodo despues
  Logger::enable(true);
  Logger::setTest(true);

  wifi.begin();
  sensorMgr.init();
  webSrv.begin();
  wsSrv.begin();

  fastTask.start();
  slowTask.start();
  wsTask.start();
}

void loop() {
  webSrv.keepAlive();
  wsSrv.keepAlive();

  static uint32_t lastSendTime = 0;
  uint32_t now = micros();

  if (now - lastSendTime >= 4000 && wsSrv.getConnectedClients() > 0) {
    portENTER_CRITICAL(&appCtx.muxSensors);
    appCtx.copyValues(sensorData);
    appCtx.resetFlags();
    appCtx.resetValues();
    portEXIT_CRITICAL(&appCtx.muxSensors);

    memset(buffer, 0, BUFFER_SIZE); //Reinicia el buffer
    sensorData.toPayload(buffer, BUFFER_SIZE); //Setea los datos en el buffer

    if (wsSrv.getConnectedClients() > 0) { //Si hay clientes conectados encola el mensaje
      if (xQueueSend(wsSrv.getQueueHandle(), buffer, 0) != pdPASS) { //Si no se pudo añadir a la cola el mensaje
        Logger::log("[WebSocket] Cola llena, paquete descartado\n"); //Manda mensaje de que la cola esta llena
      }
    }
    lastSendTime = now;
  }

  serialMgr.keepAlive();
}
