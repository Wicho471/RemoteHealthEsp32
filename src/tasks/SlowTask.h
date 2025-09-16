#ifndef SLOW_TASK_H
#define SLOW_TASK_H

#include <Arduino.h>
#include "src/core/AppContext.h"


/**
 * @brief Tarea encargada de leer sensores de baja frecuencia (temperatura y movimiento).
 */
class SlowTask {
public:
  SlowTask(AppContext& ctx);
  void start();

private:
  static void run(void* parameter);
  AppContext& context;
};

#endif // SLOW_TASK_H
