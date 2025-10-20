#include "setup_Display_S3.h"

void setup_serial() {
  if (!Serial) {
    Serial.begin(115200);
  }
  delay(100);
  Serial.println(F("Serial initialized."));
}

void setup_display() {
  setup_display_hardware();
  T_Display_S3.begin();
  T_Display_S3.clear();
  T_Display_S3.println(F("Display initialized."));
}

void setup_T_Display_S3() {
  setup_serial();
  setup_display();
  T_Display_S3.println(F("T-Display S3 setup complete."));
}