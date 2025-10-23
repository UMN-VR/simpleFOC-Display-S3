#include "foc_fault.h"
#include "motor_config.h"
#include "foc_control.h"
#include "app_config.h"
#include <Arduino.h>
#include "display_graphics.h"

namespace {
bool motorFaultLatched = false;
constexpr const char* kMotorName = "2208M0";
}

void setupFaultDetection() {
  // Set up fault pin (active low fault detection)
  pinMode(powertrain0::fault, INPUT_PULLUP);
}

bool checkAndHandleMotorFault() {
  // Check for motor fault (active low)
  const bool fault_active = digitalRead(powertrain0::fault);
  
  // Update state and handle fault transitions
  if (fault_active && !motorFaultLatched) {
    // Fault just occurred
    setFaultActiveLocked(true);
    T_Display_S3.print(" ⚠️ ");
    T_Display_S3.print(kMotorName);
    if (kDebugKeepMotorEnabled) {
      T_Display_S3.println(" fault detected (debug override keeps motor enabled)");
    } else {
      T_Display_S3.println(" disabled due to electrical fault");
    }
    motorFaultLatched = true;
  } else if (!fault_active && motorFaultLatched) {
    // Fault just cleared
    setFaultActiveLocked(false);
    T_Display_S3.print(" ✅ ");
    T_Display_S3.print(kMotorName);
    if (kDebugKeepMotorEnabled) {
      T_Display_S3.println(" fault cleared :D");
    } else {
      T_Display_S3.println(" re-enabled :D");
    }
    motorFaultLatched = false;
  }
  
  return fault_active;
}