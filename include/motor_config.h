#ifndef MOTOR_CONFIG_H
#define MOTOR_CONFIG_H

#include <Arduino.h>
#include "SimpleFOC.h"
#include "pin_config.h"

// Motor hardware configuration
namespace powertrain0 {

// Pin definitions
// Dedicated motor-control pins; keep them distinct from the LCD wiring defined in pin_config.h
constexpr uint8_t sda = PIN_MOTOR_SDA;
constexpr uint8_t scl = PIN_MOTOR_SCL;
constexpr uint8_t in1 = PIN_MOTOR_IN1;
constexpr uint8_t in2 = PIN_MOTOR_IN2;
constexpr uint8_t in3 = PIN_MOTOR_IN3;
constexpr uint8_t enable = PIN_MOTOR_ENABLE;
constexpr uint8_t fault = PIN_MOTOR_FAULT;

// Limits
constexpr uint8_t maxV = 2;
constexpr uint8_t maxA = 1;

// Motor specs (2208 12N14P 90kv motor with 7 pole pairs)
constexpr uint8_t pole_pairs = 7;

// Control type
extern MotionControlType controlType;

// Motor and driver instances
extern BLDCMotor bldc90kv2208;
extern BLDCDriver3PWM DRV8313;

}  // namespace powertrain0

#endif // MOTOR_CONFIG_H
