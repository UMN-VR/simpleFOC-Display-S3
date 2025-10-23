#pragma once

#include <Arduino.h>

namespace watchdog {

void begin();
void noteSerialTx();
void noteSerialRx();
void loop();

}  // namespace watchdog
