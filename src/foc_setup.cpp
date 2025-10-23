#include "foc_setup.h"
#include "motor_config.h"
#include "foc_control.h"
#include <Arduino.h>
#include "foc_fault.h"
#include "display_graphics.h"

namespace {
volatile bool focRuntimeReady = false;
constexpr const char* kMotorName = "2208M0";

}

void setupFOC() {
  // Set up fault detection
  setupFaultDetection();

  // Initialize the driver
  powertrain0::DRV8313.voltage_limit = powertrain0::maxV;
  powertrain0::DRV8313.init();

  // Link the motor to the driver
  powertrain0::bldc90kv2208.linkDriver(&powertrain0::DRV8313);

  // Set the motion control type to open-loop velocity
  powertrain0::bldc90kv2208.controller = MotionControlType::velocity_openloop;
  
  // Set voltage and current limits
  powertrain0::bldc90kv2208.voltage_limit = powertrain0::maxV;
  powertrain0::bldc90kv2208.current_limit = powertrain0::maxA;
  
  // Initialize the motor
  powertrain0::bldc90kv2208.init();

  // Mark FOC as ready
  focRuntimeReady = true;

  // print '{kMotorName} attached on pins IN1:\n{in1} IN2:{in2} IN3:{in3} ENABLE:{enable} FAULT:{fault}'

  T_Display_S3.println(String(kMotorName) + " attached on pins:");
  T_Display_S3.println("IN1:" + String(powertrain0::in1) +
                      " IN2:" + String(powertrain0::in2) +
                      " IN3:" + String(powertrain0::in3) +
                      " ENABLE:" + String(powertrain0::enable) +
                      " FAULT:" + String(powertrain0::fault));
  T_Display_S3.println("target velocity [rad/s] cmd format:");
  T_Display_S3.println("tv{velocity} (e.g., tv10 for 10 rad/s)");
}

void waitForFOCReady() {
  while (!focRuntimeReady) {
    delayMicroseconds(50);
  }
}

bool isFOCReady() {
  return focRuntimeReady;
}
