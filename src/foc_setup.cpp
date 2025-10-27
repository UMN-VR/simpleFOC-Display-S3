#include "foc_setup.h"
#include "motor_config.h"
#include "foc_control.h"
#include <Arduino.h>
#include <Wire.h>
#include "foc_fault.h"
#include "display_graphics.h"
#include <esp_log.h>

namespace {
volatile bool focRuntimeReady = false;
constexpr const char* kMotorName = "2208M0";
//FOCMode requestedMode = FOCMode::OpenLoop;
FOCMode requestedMode = FOCMode::ClosedLoop;
bool encoderInitialized = false;
bool encoderReady = false;
constexpr unsigned long kAngleReportIntervalMs = 1000;
unsigned long lastAngleReportMs = 0;

const char* modeName(FOCMode mode) {
  switch (mode) {
    case FOCMode::OpenLoop:
      return "OpenLoop";
    case FOCMode::ClosedLoop:
      return "ClosedLoop";
    case FOCMode::Position:
      return "Position";
  }
  return "Unknown";
}

bool ensureEncoderInitialized() {
  if (encoderInitialized && encoderReady) {
    return true;
  }

  encoderInitialized = true;

  T_Display_S3.println(F("Init AS5600..."));

  // Suppress I2C error logging to prevent serial flooding
  esp_log_level_set("i2c.master", ESP_LOG_NONE);
  esp_log_level_set("esp32-hal-i2c-ng.c", ESP_LOG_NONE);
  esp_log_level_set("Wire.cpp", ESP_LOG_NONE);

  Wire.begin(powertrain0::encoder_sda, powertrain0::encoder_scl);

  const int maxRetries = 3;
  for (int attempt = 0; attempt < maxRetries; ++attempt) {
    powertrain0::as5600.init(&Wire);
    Wire.setClock(400000);
    vTaskDelay(pdMS_TO_TICKS(5));  // Use FreeRTOS delay
    powertrain0::as5600.update();

    encoderReady = (powertrain0::as5600.currWireError == 0);
    if (encoderReady) {
      break;
    }
    T_Display_S3.print(F("AS5600 retry "));
    T_Display_S3.println(attempt + 1);
  }

  if (!encoderReady) {
    T_Display_S3.print(F("AS5600 Err:"));
    T_Display_S3.println(powertrain0::as5600.currWireError);
  }

  return encoderReady;
}
}  // namespace

FOCStatus focStatus;

void setFOCMode(FOCMode mode) {
  requestedMode = mode;
  // controlType will be set in setupFOC based on sensor availability
}

FOCMode getFOCMode() {
  return requestedMode;
}

// Modular setup functions

/**
 * Initializes the BLDC driver (DRV8313) with configured parameters.
 * @return true if initialization succeeds, false otherwise.
 */
bool initDriver() {
  powertrain0::DRV8313.voltage_limit = powertrain0::motor_config.voltage_limit;
  powertrain0::DRV8313.voltage_power_supply = powertrain0::motor_config.voltage_power_supply;
  powertrain0::DRV8313.pwm_frequency = powertrain0::motor_config.pwm_frequency;
  powertrain0::DRV8313.init();
  return true;  // Driver init always succeeds
}

/**
 * Configures the motor with voltage, current, and velocity limits.
 * @return true (configuration always succeeds).
 */
bool configureMotor() {
  powertrain0::bldc90kv2208.voltage_limit = powertrain0::motor_config.voltage_limit;
  powertrain0::bldc90kv2208.current_limit = powertrain0::motor_config.current_limit;
  powertrain0::bldc90kv2208.velocity_limit = powertrain0::motor_config.velocity_limit;
  return true;  // Configuration always succeeds
}

/**
 * Applies PID controller settings and low-pass filter to the motor.
 */
void applyPIDSettings() {
  powertrain0::bldc90kv2208.PID_velocity = powertrain0::motor_config.velocity_pid;
  powertrain0::bldc90kv2208.P_angle = powertrain0::motor_config.angle_pid;
  powertrain0::bldc90kv2208.LPF_velocity = powertrain0::motor_config.velocity_lpf;
}

/**
 * Selects the appropriate motion control type based on sensor availability and requested mode.
 * Falls back to open-loop if sensor is not available for closed-loop modes.
 * @param sensorAvailable true if encoder is ready.
 * @param mode the requested FOC mode.
 * @return the selected MotionControlType.
 */
MotionControlType selectControlStrategy(bool sensorAvailable, FOCMode mode) {
  const bool closedLoopRequested = (mode == FOCMode::ClosedLoop);
  const bool closedLoopActive = closedLoopRequested && sensorAvailable;
  const bool positionRequested = (mode == FOCMode::Position);
  const bool positionActive = positionRequested && sensorAvailable;

  if (positionActive) {
    return MotionControlType::angle;
  } else if (closedLoopActive) {
    return MotionControlType::velocity;
  } else {
    return MotionControlType::velocity_openloop;
  }
}

/**
 * Initializes the motor with the selected control type and performs FOC initialization if needed.
 * Does not enable the motor; that is deferred to the RT task.
 * @param control the motion control type.
 * @return true if initialization succeeds.
 */
bool initializeMotor(MotionControlType control) {
  powertrain0::bldc90kv2208.controller = control;
  powertrain0::controlType = control;

  powertrain0::bldc90kv2208.init();
  if (control == MotionControlType::velocity || control == MotionControlType::angle) {
    powertrain0::bldc90kv2208.initFOC();
  }

  return true;  // TODO: Add proper checks
}

/**
 * Enables the motor. Called from the RT task after setup is complete.
 */
void enableMotor() {
  powertrain0::bldc90kv2208.enable();
}

/**
 * Links the motor to the driver and sensor (if available and required).
 * @param sensorAvailable true if encoder is ready.
 * @param control the motion control type.
 */
void linkMotorAndSensor(bool sensorAvailable, MotionControlType control) {
  powertrain0::bldc90kv2208.linkDriver(&powertrain0::DRV8313);
  if (sensorAvailable && (control == MotionControlType::velocity || control == MotionControlType::angle)) {
    powertrain0::bldc90kv2208.linkSensor(&powertrain0::as5600);
  }
}

void logSetupStatus(FOCMode activeMode, bool sensorAvailable) {
  const char* activeModeName = modeName(activeMode);

  T_Display_S3.print(kMotorName);
  T_Display_S3.print(F(" "));
  T_Display_S3.println(activeModeName);
  T_Display_S3.print(F("IN:"));
  T_Display_S3.print(powertrain0::in1);
  T_Display_S3.print(F(","));
  T_Display_S3.print(powertrain0::in2);
  T_Display_S3.print(F(","));
  T_Display_S3.print(powertrain0::in3);
  T_Display_S3.print(F(" EN:"));
  T_Display_S3.print(powertrain0::enable);
  T_Display_S3.print(F(" FLT:"));
  T_Display_S3.println(powertrain0::fault);
  
  if (requestedMode == FOCMode::ClosedLoop && !sensorAvailable) {
    T_Display_S3.println(F("AS5600 N/A->OpenLoop"));
  }

  if (sensorAvailable) {
    powertrain0::as5600.update();
    const float angle = powertrain0::as5600.getAngle();
    T_Display_S3.print(F("AS5600: "));
    T_Display_S3.println(angle, 3);
    if (Serial) {
      Serial.print(F("AS5600 angle [rad]: "));
      Serial.println(angle, 4);
    }
  } else {
    T_Display_S3.println(F("AS5600 no response"));
    if (Serial) {
      Serial.println(F("AS5600 encoder not responding."));
    }
  }

  if (Serial) {
    Serial.print(F("FOC mode active: "));
    Serial.println(activeModeName);
  }
}

void setupFOC() {
  focRuntimeReady = false;

  // Set up fault detection
  setupFaultDetection();

  const bool sensorAvailable = ensureEncoderInitialized();

  // Initialize and configure driver
  if (!initDriver()) {
    T_Display_S3.println(F("Driver init failed"));
    if (Serial) Serial.println(F("Driver initialization failed"));
    return;
  }

  // Select control strategy
  MotionControlType control = selectControlStrategy(sensorAvailable, requestedMode);

  // Link motor and sensor
  linkMotorAndSensor(sensorAvailable, control);

  // Configure motor
  configureMotor();

  // Apply PID settings
  applyPIDSettings();

  // Initialize motor (without enabling)
  if (!initializeMotor(control)) {
    T_Display_S3.println(F("Motor init failed"));
    if (Serial) Serial.println(F("Motor initialization failed"));
    return;
  }

  // Mark FOC as ready
  focRuntimeReady = true;

  const FOCMode activeMode =
      (control == MotionControlType::angle) ? FOCMode::Position :
      (control == MotionControlType::velocity) ? FOCMode::ClosedLoop : FOCMode::OpenLoop;
  
  if ((requestedMode == FOCMode::ClosedLoop && !sensorAvailable) || (requestedMode == FOCMode::Position && !sensorAvailable)) {
    if (Serial) {
      Serial.println(F("Requested mode requires AS5600 but not ready; falling back to OpenLoop."));
    }
  }

  logSetupStatus(activeMode, sensorAvailable);
}

void logEncoderAngleOncePerSecond() {
  if (!focRuntimeReady) {
    return;
  }

  if (!ensureEncoderInitialized()) {
    return;
  }

  const unsigned long now = millis();
  if (now - lastAngleReportMs < kAngleReportIntervalMs) {
    return;
  }

  lastAngleReportMs = now;
  powertrain0::as5600.update();
  const float angle = powertrain0::as5600.getAngle();
  T_Display_S3.print(F("θ:"));
  T_Display_S3.println(angle, 3);
  if (Serial) {
    Serial.print(F("AS5600 angle [rad]: "));
    Serial.println(angle, 4);
  }
}

void waitForFOCReady() {
  while (!focRuntimeReady) {
    delayMicroseconds(50);
  }
}

bool isFOCReady() {
  return focRuntimeReady;
}

void setupFOC_RT() {
  focRuntimeReady = false;

  // Set up fault detection
  setupFaultDetection();

  const bool sensorAvailable = ensureEncoderInitialized();
  focStatus.sensorAvailable = sensorAvailable;

  // Initialize and configure driver
  focStatus.driverInitSuccess = initDriver();
  if (!focStatus.driverInitSuccess) {
    return;
  }

  // Select control strategy
  MotionControlType control = selectControlStrategy(sensorAvailable, requestedMode);
  focStatus.activeMode = (control == MotionControlType::angle) ? FOCMode::Position :
                         (control == MotionControlType::velocity) ? FOCMode::ClosedLoop : FOCMode::OpenLoop;

  // Link motor and sensor
  linkMotorAndSensor(sensorAvailable, control);

  // Configure motor
  configureMotor();

  // Apply PID settings
  applyPIDSettings();

  // Initialize motor (without enabling)
  focStatus.motorInitSuccess = initializeMotor(control);
  if (!focStatus.motorInitSuccess) {
    return;
  }

  // Mark FOC as ready
  focRuntimeReady = true;
  focStatus.focReady = true;
}
