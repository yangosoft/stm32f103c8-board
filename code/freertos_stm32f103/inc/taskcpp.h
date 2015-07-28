#ifndef TaskCPP_H
#define TaskCPP_H

#include "FreeRTOS.h"
#include "task.h"

class TaskBase {
public:
  xTaskHandle handle;

  ~TaskBase() {
#if INCLUDE_vTaskDelete
    vTaskDelete(handle);
#endif
    return;
  }
};

class Task : public TaskBase {
public:
  Task(char const*name, void (*taskfun)(void *), unsigned portBASE_TYPE priority,
       unsigned portSHORT stackDepth=configMINIMAL_STACK_SIZE) {
    xTaskCreate(taskfun, (signed char*)name, stackDepth, this, priority, &handle);
  }

};

