#include "display_graphics.h"
#include "display_hardware.h"
#include "pin_config.h"

#include <algorithm>

T_Display_S3_Class T_Display_S3;

void T_Display_S3_Class::begin() {
  cursor_x_ = 0;
  cursor_y_ = 0;
  text_color_ = 0xFFFF;
  bg_color_ = 0x0000;
}

void T_Display_S3_Class::clear() {
  if (!panel_handle) {
    return;
  }
  fillScreen(bg_color_);
  cursor_x_ = 0;
  cursor_y_ = 0;
}

void T_Display_S3_Class::setCursor(int16_t x, int16_t y) {
  cursor_x_ = std::max<int16_t>(0, std::min<int16_t>(x, EXAMPLE_LCD_H_RES - kGlyphWidth));
  cursor_y_ = std::max<int16_t>(0, std::min<int16_t>(y, EXAMPLE_LCD_V_RES - kGlyphHeight));
}

void T_Display_S3_Class::setTextColor(uint16_t fg, uint16_t bg) {
  text_color_ = fg;
  bg_color_ = bg;
}

void T_Display_S3_Class::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if (!panel_handle) {
    return;
  }
  if (x < 0 || x >= EXAMPLE_LCD_H_RES || y < 0 || y >= EXAMPLE_LCD_V_RES) {
    return;
  }
  uint16_t pixel = color;
  esp_lcd_panel_draw_bitmap(panel_handle, x, y, x + 1, y + 1, &pixel);
}

void T_Display_S3_Class::fillScreen(uint16_t color) {
  if (!panel_handle) {
    return;
  }
  static uint16_t line[EXAMPLE_LCD_H_RES];
  for (int x = 0; x < EXAMPLE_LCD_H_RES; ++x) {
    line[x] = color;
  }
  for (int y = 0; y < EXAMPLE_LCD_V_RES; ++y) {
    esp_lcd_panel_draw_bitmap(panel_handle, 0, y, EXAMPLE_LCD_H_RES, y + 1, line);
  }
}
