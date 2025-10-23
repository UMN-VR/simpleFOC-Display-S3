#include "watchdog.h"

#include "display_graphics.h"

namespace watchdog {

namespace {
constexpr uint32_t kIdleTimeoutMs = 10'000;
constexpr uint8_t kIdleBrightness = 64;  // 25% of 255 ≈ 64
constexpr uint8_t kActiveBrightness = 255;

uint32_t last_serial_tx = 0;
uint32_t last_serial_rx = 0;
}  // namespace

void begin() {
  auto now = millis();
  last_serial_tx = now;
  last_serial_rx = now;
}

void noteSerialTx() {
  last_serial_tx = millis();
  if (T_Display_S3.brightness() != kActiveBrightness) {
    T_Display_S3.setBrightness(kActiveBrightness);
  }
}

void noteSerialRx() {
  last_serial_rx = millis();
  if (T_Display_S3.brightness() != kActiveBrightness) {
    T_Display_S3.setBrightness(kActiveBrightness);
  }
}

void loop() {
  auto now = millis();
  uint32_t since_tx = now - last_serial_tx;
  uint32_t since_rx = now - last_serial_rx;
  if (since_tx > kIdleTimeoutMs && since_rx > kIdleTimeoutMs) {
    if (T_Display_S3.brightness() != kIdleBrightness) {
      T_Display_S3.setBrightness(kIdleBrightness);
    }
  }
}

}  // namespace watchdog
