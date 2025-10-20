#pragma once

#include <Arduino.h>
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"

// Hardware initialization functions
void setup_display_hardware();
void setBrightness(uint8_t level);

// External handles for display operations
extern esp_lcd_panel_io_handle_t io_handle;
extern esp_lcd_panel_handle_t panel_handle;
