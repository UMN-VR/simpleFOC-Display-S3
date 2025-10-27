#include <Arduino.h>
#include "setup_Display_S3.h"
#include "serial_ui.h"
#include "motor_config.h"
#include "foc_setup.h"
#include "foc_loop.h"
#include "foc_control.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void setup_background_task() {
  // Initialize display and serial on this core
  setup_T_Display_S3();
  // Initialize serial command processor
  initSerialCommandProcessor();

  // Log core ID
  T_Display_S3.print(F("BgTask->C"));
  T_Display_S3.println(xPortGetCoreID());
}


void loop_background_task() {
  // Check if FOC setup is complete and log status once
  static bool setupLogged = false;
  if (focStatus.focReady && !setupLogged) {
    logSetupStatus(focStatus.activeMode, focStatus.sensorAvailable);
    setupLogged = true;
  }

  // Check for motor faults periodically
  runFOCBackgroundLoop();

  // Process serial commands
  processSerialInput();

  // Run display activity watchdog
  T_Display_S3.watchdogCheck();

  // Update angle indicator at 10fps
  static unsigned long lastAngleUpdate = 0;
  static bool lastConnectionFailed = false;
  unsigned long now = millis();
  if (now - lastAngleUpdate >= 100) {
    if (isFOCReady()) {
      powertrain0::as5600.update();
      float angle = powertrain0::as5600.getAngle();
      bool isFailed = (powertrain0::as5600.currWireError != 0);

      if (isFailed != lastConnectionFailed) {
        if (isFailed) {
          Serial.println("AS5600 connection lost!");
        } else {
          Serial.println("AS5600 connection restored.");
        }
        lastConnectionFailed = isFailed;
      }

      T_Display_S3.drawAngleIndicator(angle, isFailed);
    }
    lastAngleUpdate = now;
  }

  // Delay to reduce CPU load for non-critical tasks
  vTaskDelay(pdMS_TO_TICKS(10));
}

// Background task for serial UI, fault checking, and watchdog
void backgroundTask(void *pvParameters) {
  (void)pvParameters;

  setup_background_task();

  for (;;) {
    loop_background_task();
  }
}