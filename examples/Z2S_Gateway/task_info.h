#ifndef TASK_INFO_H
#define TASK_INFO_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

/*****************************************************************************/

#define MAX_MONITORED_TASKS 12

/*****************************************************************************/

struct TaskHistory {
  TaskHandle_t xHandle;
  uint32_t ulRunTimeCounter;
};

/*****************************************************************************/

class TaskInfo {
  private:

    TaskStatus_t taskStatusArray[MAX_MONITORED_TASKS];
    TaskHistory prevTaskHistory[MAX_MONITORED_TASKS];
    UBaseType_t prevTaskCount = 0;
    uint32_t prevTotalRunTime = 0;

    float cpuUsage = 0.0f;

  public:
    void get(char *task_info_buffer, size_t buffer_size);
    float getCpuUsage() const { return cpuUsage; }
};

/*****************************************************************************/

void cpuMonitorTimerCallback(TimerHandle_t xTimer);
void setupCpuMonitoring();

/*****************************************************************************/

extern TaskInfo GatewayTaskInfo;

#endif // TASK_INFO_H