#ifndef FAST_TASK_H
#define FAST_TASK_H

#include <Arduino.h>
#include "src/core/AppContext.h"

/**
 * @brief Tarea encargada de leer ECG y MAX30105 (alta frecuencia).
 */
class FastTask {
public:
  explicit FastTask(AppContext& ctx);
  void start();

private:
  static void run(void* parameter);
  AppContext& context;
};

#endif // FAST_TASK_H
