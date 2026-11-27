#include "task_info.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <Arduino.h>

/*****************************************************************************/

static void appendFmt(
  char *buf, size_t buf_size, size_t &offset, const char *fmt, ...) {

  if (!buf || buf_size == 0 || offset >= (buf_size - 1))
    return;

  va_list args;
  va_start(args, fmt);
  int written = vsnprintf(buf + offset, buf_size - offset, fmt, args);
  va_end(args);

  if (written > 0) {
    size_t avail = buf_size - offset;
    offset += ((size_t)written < avail) ? (size_t)written : (avail - 1);
  }
}

/*****************************************************************************/

TaskInfo::TaskInfo() {
  _mutex = xSemaphoreCreateMutexStatic(&_mutex_buf);
}

/*****************************************************************************/

void TaskInfo::get(char *task_info_buffer, size_t buffer_size) {

  if (_mutex)
    xSemaphoreTake(_mutex, portMAX_DELAY);

  uint32_t ulTotalRunTime = 0;
  size_t offset = 0;

  if (task_info_buffer && buffer_size > 0) {
    task_info_buffer[0] = '\0';
  }

  UBaseType_t uxArraySize = uxTaskGetSystemState(
    taskStatusArray, MAX_MONITORED_TASKS, &ulTotalRunTime);

  if (uxArraySize == 0) {
    appendFmt(
      task_info_buffer, buffer_size, offset,
      "Too many tasks (> %u)!\n\r", (unsigned)MAX_MONITORED_TASKS);
    if (_mutex) xSemaphoreGive(_mutex);
    return;
  }

  uint32_t deltaTotalRunTime = ulTotalRunTime - prevTotalRunTime;

  if (deltaTotalRunTime == 0) {
    appendFmt(
      task_info_buffer, buffer_size, offset,
      "Sampling interval too short.\n\r");
    if (_mutex) xSemaphoreGive(_mutex);
    return;
  }

  appendFmt(
    task_info_buffer, buffer_size, offset,
    "\n\rTask            | CPU%%  |Pr|C|MinFree\n\r");

  TaskHistory nextTaskHistory[MAX_MONITORED_TASKS];
  float idlePercentSum = 0.0f;
  uint8_t idleCount = 0;

  for (UBaseType_t i = 0; i < uxArraySize; i++) {

    TaskHandle_t handle = taskStatusArray[i].xHandle;
    uint32_t currentTaskTime = taskStatusArray[i].ulRunTimeCounter;
    uint32_t prevTaskTime = 0;

    for (UBaseType_t j = 0; j < prevTaskCount; j++) {
      if (prevTaskHistory[j].xHandle == handle) {
        prevTaskTime = prevTaskHistory[j].ulRunTimeCounter;
        break;
      }
    }

    uint32_t deltaTaskTime = currentTaskTime - prevTaskTime;
    float percentage = ((float)deltaTaskTime * 100.0f) /
      (float)deltaTotalRunTime;

    if (strncmp(taskStatusArray[i].pcTaskName, "IDLE", 4) == 0) {
      idlePercentSum += percentage;
      idleCount++;
    }

    uint32_t minFreeStackBytes =
      (uint32_t)taskStatusArray[i].usStackHighWaterMark * sizeof(StackType_t);

    BaseType_t coreId = xTaskGetCoreID(handle);
    char coreCh = (coreId >= 0 && coreId < 10) ? (char)('0' + coreId) : '*';

    appendFmt(
      task_info_buffer, buffer_size, offset,
      "%-15.15s |%6.2f |%2u|%c|%6lu\n\r",
      taskStatusArray[i].pcTaskName,
      percentage,
      (unsigned)taskStatusArray[i].uxCurrentPriority,
      coreCh,
      (unsigned long)minFreeStackBytes);

    nextTaskHistory[i].xHandle = handle;
    nextTaskHistory[i].ulRunTimeCounter = currentTaskTime;
  }

  if (idleCount > 0) {
    float calculatedCpu = 100.0f - (idlePercentSum / (float)idleCount);
    if (calculatedCpu < 0.0f) calculatedCpu = 0.0f;
    if (calculatedCpu > 100.0f) calculatedCpu = 100.0f;
    cpuUsage = calculatedCpu;
  }

  appendFmt(
    task_info_buffer, buffer_size, offset,
    "CPU: %4.1f%% | Tasks: %u/%u (MinFree=B)\n\r",
    cpuUsage, (unsigned)uxArraySize, (unsigned)MAX_MONITORED_TASKS);

  for (UBaseType_t i = 0; i < uxArraySize; i++) {
    prevTaskHistory[i] = nextTaskHistory[i];
  }
  prevTaskCount = uxArraySize;
  prevTotalRunTime = ulTotalRunTime;

  if (_mutex)
    xSemaphoreGive(_mutex);
}

/*****************************************************************************/

void cpuMonitorTimerCallback(TimerHandle_t xTimer) {
  (void)xTimer;
  GatewayTaskInfo.get(nullptr, 0);
}

/*****************************************************************************/

void setupCpuMonitoring() {

  TimerHandle_t cpuTimer = xTimerCreate(
    "CpuMonTimer", pdMS_TO_TICKS(60000), pdTRUE, nullptr,
    cpuMonitorTimerCallback);

  if (cpuTimer != NULL)
    xTimerStart(cpuTimer, 0);
  else
    log_e("xTimerStart for CPU monitoring has failed!");
}

TaskInfo GatewayTaskInfo;