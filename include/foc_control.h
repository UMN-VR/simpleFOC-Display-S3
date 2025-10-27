#ifndef FOC_CONTROL_H
#define FOC_CONTROL_H

#include <Arduino.h>


constexpr bool kDebugKeepMotorEnabled = true; // Leave false so faults genuinely gate the motor.

struct FOCSharedState {
  float target_velocity;
  float target_angle;
  bool motor_enabled;
  bool fault_active;
};

extern FOCSharedState focState;

void writeTargetVelocityLocked(float velocity);
void writeTargetAngleLocked(float angle);
void setFaultActiveLocked(bool active);
FOCSharedState readFocStateSnapshot();

#endif