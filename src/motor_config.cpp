#include "motor_config.h"

namespace powertrain0 {

MotionControlType controlType = MotionControlType::velocity_openloop;

// BLDC motor instance
BLDCMotor bldc90kv2208 = BLDCMotor(powertrain0::pole_pairs);

// BLDC driver instance
BLDCDriver3PWM DRV8313(powertrain0::in1, powertrain0::in2, powertrain0::in3, powertrain0::enable);

}  // namespace powertrain0
