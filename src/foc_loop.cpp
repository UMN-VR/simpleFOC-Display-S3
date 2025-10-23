#include "foc_loop.h"
#include "motor_config.h"
#include "foc_control.h"
#include "foc_fault.h"

void runFOCLoop() {
  // Check and handle motor fault
  checkAndHandleMotorFault();

  // Read the current FOC state (thread-safe)
  const FOCSharedState state = readFocStateSnapshot();
  
  // Apply motor movement (only if enabled)
  powertrain0::bldc90kv2208.move(state.motor_enabled ? state.target_velocity : 0.0f);
}

void runRealTimeFOCLoop() {
  powertrain0::bldc90kv2208.loopFOC();
}