#pragma once

#include <LovyanGFX.hpp>
#include "pin_config.h"

enum class DisplayTheme {
  Light,
  Dark,
};

class T_Display_S3_Class : public lgfx::LGFX_Device {
 public:
  T_Display_S3_Class();

  void begin();
  void clear();
  void clear(uint16_t color);
  void setTheme(DisplayTheme theme);
  void toggleTheme();
  void setBrightness(uint8_t brightness);
  uint8_t brightness() const;
  void noteSerialReceived();
  void watchdogCheck();
  DisplayTheme theme() const;
  uint16_t foregroundColor() const;
  uint16_t backgroundColor() const;
  void drawAngleIndicator(float angle_rad, bool isFailed = false);
  void drawWarningEmoji(int x, int y);
  void drawCheckEmoji(int x, int y);

  // Override print methods to mirror to Serial
  size_t write(uint8_t c) override;
  size_t write(const uint8_t *buffer, size_t size) override;

 private:
  void configureBus();
  void configurePanel();
  void applyTheme(DisplayTheme theme);

  lgfx::Bus_Parallel8 bus_;
  lgfx::Panel_ST7789 panel_;
  lgfx::Light_PWM light_;
  DisplayTheme theme_ = DisplayTheme::Light;
  uint16_t foreground_color_ = 0x0000u;
  uint16_t background_color_ = 0xFFFFu;
  uint8_t brightness_ = 255u;
  // Watchdog / activity tracking
  unsigned long last_serial_sent_ms_ = 0;
  unsigned long last_serial_received_ms_ = 0;
  uint8_t previous_brightness_ = 255u;

  // Watchdog config
  static constexpr unsigned long kInactivityTimeoutMs = 10000ul; // 10 seconds
  static constexpr uint8_t kReducedBrightness = 64u; // ~25%

  // Watchdog API
  void noteSerialSent();
};

extern T_Display_S3_Class T_Display_S3;
