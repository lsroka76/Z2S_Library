#include "task_info.h"
#include <stdio.h>
#include <Arduino.h>

/*****************************************************************************/

void TaskInfo::get(char *task_info_buffer, size_t buffer_size) {

  uint32_t ulTotalRunTime;
  uint32_t task_info_buffer_offset = 0;

  UBaseType_t uxArraySize = uxTaskGetSystemState(
    taskStatusArray, MAX_MONITORED_TASKS, &ulTotalRunTime);

  
  uint32_t deltaTotalRunTime = ulTotalRunTime - prevTotalRunTime;

  if (deltaTotalRunTime == 0) {
    
    if (task_info_buffer)
      snprintf(
        task_info_buffer, buffer_size, "Sampling interval too short.\n\r");
    return;
  }

  if (task_info_buffer)
    task_info_buffer_offset += snprintf(
      task_info_buffer, buffer_size - task_info_buffer_offset, 
      "\n\r--- Task CPU Usage (Since Last Call) ---\n\r");

  TaskHandle_t xIdleHandle = xTaskGetIdleTaskHandle();
  uint32_t idleDeltaTime = 0;
  TaskHistory nextTaskHistory[MAX_MONITORED_TASKS];

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

    if (handle == xIdleHandle) {

      float calculatedCpu = 100.0f - percentage;

      if (calculatedCpu < 0.0f) calculatedCpu = 0.0f;
      if (calculatedCpu > 100.0f) calculatedCpu = 100.0f;

      cpuUsage = calculatedCpu;
    }

    if (task_info_buffer)
      task_info_buffer_offset += snprintf(
        task_info_buffer + task_info_buffer_offset, 
        buffer_size - task_info_buffer_offset,
        "Task: %-16s | CPU: %6.2f%%\n\r", taskStatusArray[i].pcTaskName, 
      percentage);

        
    nextTaskHistory[i].xHandle = handle;
    nextTaskHistory[i].ulRunTimeCounter = currentTaskTime;
  }

    
  for (UBaseType_t i = 0; i < uxArraySize; i++) {
      
      prevTaskHistory[i] = nextTaskHistory[i];
  }
  prevTaskCount = uxArraySize;
  prevTotalRunTime = ulTotalRunTime;
}

/*****************************************************************************/

void cpuMonitorTimerCallback(TimerHandle_t xTimer) {
    
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