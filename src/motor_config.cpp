#include "motor_config.h"

namespace powertrain0 {

MotionControlType controlType = MotionControlType::velocity_openloop;

// Default motor configuration
MotorConfig motor_config = {
    .voltage_limit = maxV,  // 2V
    .voltage_power_supply = 12.0f,
    .pwm_frequency = 25000,
    .current_limit = maxA,  // 1A
    .velocity_limit = 20.0f,
    .velocity_pid = {0.2f, 20.0f, 0.0f, 1000.0f, maxV},
    .angle_pid = {5.0f, 0.0f, 0.0f, 1000.0f, maxV},
    .velocity_lpf = {0.01f}
};

// BLDC motor instance
BLDCMotor bldc90kv2208 = BLDCMotor(powertrain0::pole_pairs);

// BLDC driver instance
BLDCDriver3PWM DRV8313(powertrain0::in1, powertrain0::in2, powertrain0::in3, powertrain0::enable);

// Magnetic encoder instance (AS5600 over I2C)
MagneticSensorI2C as5600 = MagneticSensorI2C(AS5600_I2C);

}  // namespace powertrain0
