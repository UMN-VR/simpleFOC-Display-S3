#include <Arduino.h>
#include "setup_Display_S3.h"
#include "foc_loop.h"
#include "background_task.h"
#include "rt_foc_task.h"
#include <esp_log.h>

// FreeRTOS for task creation and core pinning (ESP32)
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Configure which core the real-time FOC task should run on.
// Core 1 for RT FOC, Core 0 for background tasks.
#ifndef RT_FOC_CORE
#define RT_FOC_CORE 1
#endif

#ifndef BG_TASK_CORE
#define BG_TASK_CORE 0
#endif

// Stack size and priority for the real-time task. Adjust if you see
// stack overflows or need different scheduling priority.
#ifndef RT_FOC_STACK_SIZE
#define RT_FOC_STACK_SIZE 4096
#endif
#ifndef RT_FOC_PRIORITY
#define RT_FOC_PRIORITY 5
#endif

#ifndef BG_TASK_STACK_SIZE
#define BG_TASK_STACK_SIZE 8192
#endif
#ifndef BG_TASK_PRIORITY
#define BG_TASK_PRIORITY 1
#endif


void startBackgroundTask(){

  BaseType_t bgRc = xTaskCreatePinnedToCore(
      backgroundTask,      // task function
      "Background",        // name
      BG_TASK_STACK_SIZE,  // stack size (bytes)
      NULL,                // parameters
      BG_TASK_PRIORITY,    // priority
      NULL,                // task handle (not needed)
      BG_TASK_CORE);       // core id
  if (bgRc != pdPASS) {
    T_Display_S3.println(F(" FAIL"));
  } else {
    T_Display_S3.println(F(" OK"));
  }

}

void start_RT_FOC_Task(){

    BaseType_t rc = xTaskCreatePinnedToCore(
      RT_FOC_Task,           // task function
      "RTFOC",            // name
      RT_FOC_STACK_SIZE,   // stack size (bytes)
      NULL,                // parameters
      RT_FOC_PRIORITY,     // priority
      NULL,                // task handle (not needed)
      RT_FOC_CORE);        // core id
  if (rc != pdPASS) {
    T_Display_S3.println(F(" FAIL"));
  } else {
    T_Display_S3.println(F(" OK"));
  }

}
  

void setup() {

  // Suppress all ESP_LOG messages to prevent serial flooding
  esp_log_level_set("*", ESP_LOG_NONE);

  startBackgroundTask();

  start_RT_FOC_Task();

}

void loop() {
  // End the Arduino loop task - we must take full control of both cores 
  vTaskDelete(NULL);
}