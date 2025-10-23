#include "display_hardware.h"
#include "pin_config.h"

void setup_display_hardware() {
  pinMode(PIN_POWER_ON, OUTPUT);
  digitalWrite(PIN_POWER_ON, HIGH);

  pinMode(PIN_LCD_RD, OUTPUT);
  digitalWrite(PIN_LCD_RD, HIGH);
  pinMode(PIN_LCD_BL, OUTPUT);
  digitalWrite(PIN_LCD_BL, HIGH);
}

void setBrightness(uint8_t value) {
  analogWrite(PIN_LCD_BL, value);
}
