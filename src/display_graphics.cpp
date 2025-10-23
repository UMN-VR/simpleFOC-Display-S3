#include "display_graphics.h"
#include "display_hardware.h"
#include <Arduino.h>

namespace {
constexpr uint16_t kColorBlack = 0x0000u;
constexpr uint16_t kColorWhite = 0xFFFFu;
}

T_Display_S3_Class T_Display_S3;

T_Display_S3_Class::T_Display_S3_Class() {
  configureBus();
  configurePanel();
  setPanel(&panel_);
}

void T_Display_S3_Class::configureBus() {
  auto cfg = bus_.config();
  cfg.freq_write = EXAMPLE_LCD_PIXEL_CLOCK_HZ;
  cfg.pin_wr = PIN_LCD_WR;
  cfg.pin_rd = PIN_LCD_RD;
  cfg.pin_rs = PIN_LCD_DC;
  cfg.pin_d0 = PIN_LCD_D0;
  cfg.pin_d1 = PIN_LCD_D1;
  cfg.pin_d2 = PIN_LCD_D2;
  cfg.pin_d3 = PIN_LCD_D3;
  cfg.pin_d4 = PIN_LCD_D4;
  cfg.pin_d5 = PIN_LCD_D5;
  cfg.pin_d6 = PIN_LCD_D6;
  cfg.pin_d7 = PIN_LCD_D7;
  bus_.config(cfg);
  panel_.setBus(&bus_);
}

void T_Display_S3_Class::configurePanel() {
  auto cfg = panel_.config();
  cfg.pin_cs = PIN_LCD_CS;
  cfg.pin_rst = PIN_LCD_RES;
  cfg.pin_busy = -1;
  cfg.memory_width = 240;
  cfg.memory_height = 320;
  cfg.panel_width = 170;
  cfg.panel_height = 320;
  cfg.offset_x = 35;
  cfg.offset_y = 0;
  cfg.offset_rotation = 0;
  cfg.dummy_read_pixel = 8;
  cfg.dummy_read_bits = 1;
  cfg.readable = false;
  cfg.invert = true;
  cfg.rgb_order = false;
  cfg.dlen_16bit = false;
  cfg.bus_shared = false;
  panel_.config(cfg);

  auto light_cfg = light_.config();
  light_cfg.pin_bl = PIN_LCD_BL;
  light_cfg.freq = 12000;
  light_cfg.pwm_channel = 0;
  light_cfg.invert = false;
  light_.config(light_cfg);
  panel_.setLight(&light_);
}

void T_Display_S3_Class::begin() {
  setup_display_hardware();
  init();
  setRotation(1);
  setBrightness(255);
  setTheme(DisplayTheme::Light);
  Serial.println("Display init complete");
}

void T_Display_S3_Class::clear() {
  fillScreen(background_color_);
}

void T_Display_S3_Class::clear(uint16_t color) {
  fillScreen(color);
}

void T_Display_S3_Class::setTheme(DisplayTheme theme) {
  applyTheme(theme);
  clear();
  setCursor(0, 0);
}

void T_Display_S3_Class::toggleTheme() {
  auto next_theme = theme_ == DisplayTheme::Light ? DisplayTheme::Dark : DisplayTheme::Light;
  setTheme(next_theme);
}

DisplayTheme T_Display_S3_Class::theme() const {
  return theme_;
}

uint16_t T_Display_S3_Class::foregroundColor() const {
  return foreground_color_;
}

uint16_t T_Display_S3_Class::backgroundColor() const {
  return background_color_;
}

void T_Display_S3_Class::applyTheme(DisplayTheme theme) {
  theme_ = theme;

  if (theme_ == DisplayTheme::Light) {
    foreground_color_ = kColorBlack;
    background_color_ = kColorWhite;
  } else {
    foreground_color_ = kColorWhite;
    background_color_ = kColorBlack;
  }

  setTextColor(foreground_color_, background_color_);
}

size_t T_Display_S3_Class::write(uint8_t c) {
  Serial.write(c);
  // note that we sent serial data
  noteSerialSent();

  size_t result = LGFX_Device::write(c);
  
  // Get font height to check if we're on the last line
  int32_t font_height = fontHeight();
  
  // If cursor is on or past the last line, clear and reset
  if (getCursorY() + font_height >= height()) {
    clear();
    setCursor(0, 0);
    setTextColor(foreground_color_, background_color_);
  }
  
  return result;
}

size_t T_Display_S3_Class::write(const uint8_t *buffer, size_t size) {
  Serial.write(buffer, size);
  // mark activity (bulk write)
  noteSerialSent();
  return LGFX_Device::write(buffer, size);
}

void T_Display_S3_Class::noteSerialSent() {
  last_serial_sent_ms_ = millis();
}

void T_Display_S3_Class::noteSerialReceived() {
  last_serial_received_ms_ = millis();
}

void T_Display_S3_Class::watchdogCheck() {
  unsigned long now = millis();
  unsigned long last_activity = last_serial_sent_ms_ > last_serial_received_ms_ ? last_serial_sent_ms_ : last_serial_received_ms_;

  if (now - last_activity >= kInactivityTimeoutMs) {
    // reduce brightness if not already reduced
    if (brightness_ != kReducedBrightness) {
      previous_brightness_ = brightness_;
      setBrightness(kReducedBrightness);
    }
  } else {
    // restore brightness if it was reduced
    if (brightness_ == kReducedBrightness && previous_brightness_ != brightness_) {
      setBrightness(previous_brightness_);
    }
  }
}

void T_Display_S3_Class::setBrightness(uint8_t brightness) {
  brightness_ = brightness;
  ::setBrightness(brightness_);
  LGFX_Device::setBrightness(brightness_);
}

uint8_t T_Display_S3_Class::brightness() const {
  return brightness_;
}
