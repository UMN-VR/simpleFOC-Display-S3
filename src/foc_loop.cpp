#include "foc_loop.h"
#include "motor_config.h"
#include "foc_control.h"
#include "foc_fault.h"

void runFOCBackgroundLoop() {
  // Check and handle motor fault (do this less frequently)
  static unsigned long lastFaultCheck = 0;
  unsigned long now = millis();
  if (now - lastFaultCheck >= 10) { // Check every 10ms
    checkAndHandleMotorFault();
    lastFaultCheck = now;
  }
}

void runRealTimeFOCLoop() {

  
  // Read the current FOC state (thread-safe)
  const FOCSharedState state = readFocStateSnapshot();
  
  // Apply motor movement (only if enabled)
  float target = 0.0f;
  if (powertrain0::bldc90kv2208.controller == MotionControlType::angle) {
    target = state.target_angle;
  } else {
    target = state.target_velocity;
  }

  // Call loopFOC and move together for proper timing
  powertrain0::bldc90kv2208.loopFOC();

  powertrain0::bldc90kv2208.move(state.motor_enabled ? target : 0.0f);
}