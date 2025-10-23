#include <Arduino.h>
#include "setup_Display_S3.h"
#include "loop_Display_S3.h"


#include "serial_ui.h"
#include "motor_config.h"
#include "foc_setup.h"
#include "foc_loop.h"
#include "foc_control.h"

void setup() {
  // put your setup code here, to run once:
  setup_T_Display_S3();


  // Initialize FOC motor and driver
  setupFOC();

  // Initialize serial command processor
  initSerialCommandProcessor();
}




void loop() {
  // put your main code here, to run repeatedly:
  //loop_T_Display_S3(); //use for demo purposes
  processSerialInput();
  // Run display activity watchdog
  T_Display_S3.watchdogCheck();
}