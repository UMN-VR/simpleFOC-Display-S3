#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "foc_loop.h"
#include "foc_setup.h"

// Wrapper task that pins runRealTimeFOCLoop to a dedicated core.
void RT_FOC_Task(void *pvParameters) {
  (void)pvParameters;

  // Perform FOC setup on RT core
  setupFOC_RT();

  // Enable the motor on RT core
  enableMotor();

  for (;;) {
    // Call the real-time FOC function in a tight loop for smooth operation
    // loopFOC() should run as fast as possible (typically 10-100 kHz)
    runRealTimeFOCLoop();
    // Use taskYIELD instead of vTaskDelay to allow other same-priority tasks
    // but keep the loop running as fast as possible
    taskYIELD();
  }
}