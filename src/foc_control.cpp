#include "foc_control.h"

FOCSharedState focState = {
    /*target_velocity=*/0.0f,
    /*motor_enabled=*/true,
    /*fault_active=*/false,
};

FOCSharedState readFocStateSnapshot() {
  
  const FOCSharedState snapshot = focState;
  
  return snapshot;
}

void writeTargetVelocityLocked(float velocity) {
  
  focState.target_velocity = velocity;
  focState.motor_enabled = kDebugKeepMotorEnabled || !focState.fault_active;
    
}

void setFaultActiveLocked(bool active) {
  
  focState.fault_active = active;
  if (!kDebugKeepMotorEnabled) {
    if (active) {
      focState.motor_enabled = false;
      focState.target_velocity = 0.0f;
    } else {
      focState.motor_enabled = true;
    }
  } else {
    focState.motor_enabled = true;
  }
}
