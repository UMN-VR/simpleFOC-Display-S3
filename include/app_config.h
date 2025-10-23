#pragma once
#include <stdint.h>

namespace AppConfig {

inline constexpr unsigned long baudRate = 115200;
inline constexpr unsigned long helloIntervalMs = 1000;
inline constexpr unsigned long heartbeatIntervalMs = 500;
inline constexpr unsigned long serialWaitTimeoutMs = 5000;


inline constexpr char repoUrl[] = "https://github.com/Felipegalind0/rp2040-zero-docs-pinout-platformio-project";
inline constexpr char helloMessage[] = "Hello from RP2040 over USB serial!";

}  // namespace AppConfig