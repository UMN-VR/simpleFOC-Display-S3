#pragma once

#include <Arduino.h>
#include "pin_config.h"

class T_Display_S3_Class : public Print {
 public:
  void begin();
  void clear();
  void setCursor(int16_t x, int16_t y);
  void setTextColor(uint16_t fg, uint16_t bg = 0x0000);

  using Print::print;
  using Print::println;

  size_t write(uint8_t c) override;

  // Graphics primitives
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void fillScreen(uint16_t color);

  // Constants
  static constexpr int16_t kGlyphWidth = 6;   // character width with spacing
  static constexpr int16_t kGlyphHeight = 8;  // character height with spacing

 private:
  friend class T_Display_S3_Text;  // Allow text module to access internals

  int16_t cursor_x_ = 0;
  int16_t cursor_y_ = 0;
  uint16_t text_color_ = 0xFFFF;
  uint16_t bg_color_ = 0x0000;

  // Text rendering helpers (implemented in display_text.cpp)
  void newLine();
  bool drawGlyph(char c);
};

// Global instance
extern T_Display_S3_Class T_Display_S3;
