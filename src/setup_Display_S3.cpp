#include "setup_Display_S3.h"

void setup_serial() {
  if (!Serial) {
    Serial.begin(115200);
  }
  delay(100);
  Serial.println(F("Serial initialized."));
}

void setup_display() {
  T_Display_S3.begin();
  // set brightness to 50%
  T_Display_S3.setBrightness(128);
}

void setup_T_Display_S3() {
  setup_serial();
  setup_display();
  
  // Compact system info
  T_Display_S3.print(F("Core:"));
  T_Display_S3.print(xPortGetCoreID());
  T_Display_S3.print(F(" "));
  T_Display_S3.print(getCpuFrequencyMhz());
  T_Display_S3.print(F("MHz XTAL:"));
  T_Display_S3.print(getXtalFrequencyMhz());
  T_Display_S3.println(F("MHz"));
}