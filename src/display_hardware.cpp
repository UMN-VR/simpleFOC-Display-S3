#include "display_hardware.h"
#include "pin_config.h"

#include <esp_err.h>

esp_lcd_panel_io_handle_t io_handle = nullptr;
esp_lcd_panel_handle_t panel_handle = nullptr;

namespace {

struct lcd_cmd_t {
  uint8_t cmd;
  uint8_t data[14];
  uint8_t len;
};

constexpr lcd_cmd_t kPanelInitCommands[] = {
    {0x11, {0}, 0x80},
    {0x3A, {0x05}, 1},
    {0xB2, {0x0B, 0x0B, 0x00, 0x33, 0x33}, 5},
    {0xB7, {0x75}, 1},
    {0xBB, {0x28}, 1},
    {0xC0, {0x2C}, 1},
    {0xC2, {0x01}, 1},
    {0xC3, {0x1F}, 1},
    {0xC6, {0x13}, 1},
    {0xD0, {0xA7}, 1},
    {0xD0, {0xA4, 0xA1}, 2},
    {0xD6, {0xA1}, 1},
    {0xE0, {0xF0, 0x05, 0x0A, 0x06, 0x06, 0x03, 0x2B, 0x32, 0x43, 0x36, 0x11, 0x10, 0x2B, 0x32}, 14},
    {0xE1, {0xF0, 0x08, 0x0C, 0x0B, 0x09, 0x24, 0x2B, 0x22, 0x43, 0x38, 0x15, 0x16, 0x2F, 0x37}, 14},
};

void log_if_error(const __FlashStringHelper *message, esp_err_t err) {
  if (err == ESP_OK) {
    return;
  }
  Serial.print(message);
  Serial.print(F(" failed, err=0x"));
  Serial.println(err, HEX);
}

}  // namespace

void setBrightness(uint8_t value) {
  static uint8_t level = 0;
  constexpr uint8_t kSteps = 16;

  if (value == 0) {
    digitalWrite(PIN_LCD_BL, LOW);
    delay(3);
    level = 0;
    return;
  }

  if (level == 0) {
    digitalWrite(PIN_LCD_BL, HIGH);
    level = kSteps;
    delayMicroseconds(30);
  }

  const int from = kSteps - level;
  const int to = kSteps - value;
  const int pulses = (kSteps + to - from) % kSteps;
  for (int i = 0; i < pulses; i++) {
    digitalWrite(PIN_LCD_BL, LOW);
    digitalWrite(PIN_LCD_BL, HIGH);
  }

  level = value;
}

void setup_display_hardware() {
  pinMode(PIN_POWER_ON, OUTPUT);
  digitalWrite(PIN_POWER_ON, HIGH);

  pinMode(PIN_LCD_RD, OUTPUT);
  digitalWrite(PIN_LCD_RD, HIGH);

  esp_lcd_i80_bus_handle_t bus_handle = nullptr;

  const esp_lcd_i80_bus_config_t bus_config = {
      .dc_gpio_num = PIN_LCD_DC,
      .wr_gpio_num = PIN_LCD_WR,
      .clk_src = LCD_CLK_SRC_PLL160M,
      .data_gpio_nums = {PIN_LCD_D0, PIN_LCD_D1, PIN_LCD_D2, PIN_LCD_D3, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7},
      .bus_width = 8,
      .max_transfer_bytes = LVGL_LCD_BUF_SIZE * sizeof(uint16_t),
      .psram_trans_align = 0,
      .sram_trans_align = 0};

  esp_err_t err = esp_lcd_new_i80_bus(&bus_config, &bus_handle);
  log_if_error(F("esp_lcd_new_i80_bus"), err);
  if (err != ESP_OK) {
    return;
  }

  const esp_lcd_panel_io_i80_config_t io_config = {
      .cs_gpio_num = PIN_LCD_CS,
      .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
      .trans_queue_depth = 10,
      .on_color_trans_done = nullptr,
      .user_ctx = nullptr,
      .lcd_cmd_bits = 8,
      .lcd_param_bits = 8,
      .dc_levels = {.dc_idle_level = 0, .dc_cmd_level = 0, .dc_dummy_level = 0, .dc_data_level = 1},
  };

  err = esp_lcd_new_panel_io_i80(bus_handle, &io_config, &io_handle);
  log_if_error(F("esp_lcd_new_panel_io_i80"), err);
  if (err != ESP_OK) {
    return;
  }

  const esp_lcd_panel_dev_config_t panel_config = {
      .reset_gpio_num = PIN_LCD_RES,
      .color_space = ESP_LCD_COLOR_SPACE_RGB,
      .bits_per_pixel = 16,
      .vendor_config = nullptr};

  err = esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle);
  log_if_error(F("esp_lcd_new_panel_st7789"), err);
  if (err != ESP_OK) {
    return;
  }

  esp_lcd_panel_reset(panel_handle);
  err = esp_lcd_panel_init(panel_handle);
  log_if_error(F("esp_lcd_panel_init"), err);
  if (err != ESP_OK) {
    return;
  }

  esp_lcd_panel_invert_color(panel_handle, true);
  esp_lcd_panel_swap_xy(panel_handle, true);
  esp_lcd_panel_mirror(panel_handle, false, true);
  esp_lcd_panel_set_gap(panel_handle, 0, 35);

  for (const auto &cmd : kPanelInitCommands) {
    esp_lcd_panel_io_tx_param(io_handle, cmd.cmd, cmd.data, cmd.len & 0x7F);
    if (cmd.len & 0x80) {
      delay(120);
    }
  }

  pinMode(PIN_LCD_BL, OUTPUT);
  setBrightness(12);

  esp_lcd_panel_disp_on_off(panel_handle, true);
}
