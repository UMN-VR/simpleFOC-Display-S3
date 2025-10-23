#include "serial_ui.h"

#include <Arduino.h>

#include "app_config.h"
#include "foc_control.h"
#include "display_graphics.h"

void printStartupBanner() {
  T_Display_S3.println();
  T_Display_S3.println("=============================================");
  T_Display_S3.println(" RP2040 / ESP32 CPU Benchmark (QR Code Gen)");
  T_Display_S3.println("=============================================");
  T_Display_S3.println("USB serial link is ready.");
  T_Display_S3.print("Project repo (and QR source): ");
  T_Display_S3.println(AppConfig::repoUrl);
  T_Display_S3.println();
}

// Serial command state machine
namespace {
enum class SerialState {
  S0_IDLE,      // Waiting for 't', 'd', or 'c'
  S1_GOT_T,     // Got 't', waiting for 'v'
  S2_GOT_TV,    // Got 'tv', collecting digits
  S1_GOT_D,     // Got 'd', waiting for 'v'
  S2_GOT_DV,    // Got 'dv', waiting for 'f'
  S1_GOT_C,     // Got 'c', waiting for 'l'
};

SerialState currentState = SerialState::S0_IDLE;
String serialCmdInputBuffer;
constexpr size_t MAX_BUFFER_SIZE = 16; // Prevent buffer overflow
}  // namespace

void initSerialCommandProcessor() {
  serialCmdInputBuffer.reserve(MAX_BUFFER_SIZE);
  currentState = SerialState::S0_IDLE;
  serialCmdInputBuffer = "";
}

void printInvalidInputWarning() {
  T_Display_S3.println("Invalid input, available commands: tv<-100.0 to 100.0>, dvf, cl");
}

void processSerialInput() {
  while (Serial.available() > 0) {
    
  char c = Serial.read();
  // notify display watchdog of incoming serial
  T_Display_S3.noteSerialReceived();
    //T_Display_S3.print(String(static_cast<int>(currentState)) + ": Read char '" + c + "'\n");
    
    switch (currentState) {
      case SerialState::S0_IDLE:
        if (c == 't'|| c == 'T') {
          serialCmdInputBuffer = "t";
          T_Display_S3.print(c); // Echo the character
          currentState = SerialState::S1_GOT_T;
        } else if (c == 'd' || c == 'D') {
          serialCmdInputBuffer = "d";
          T_Display_S3.print(c); // Echo the character
          currentState = SerialState::S1_GOT_D;
        } else if (c == 'c' || c == 'C') {
          serialCmdInputBuffer = "c";
          T_Display_S3.print(c); // Echo the character
          currentState = SerialState::S1_GOT_C;
        } else if (c == '\n' || c == '\r') {
          // Silently ignore newline characters when idle
        } else {
          // Ignore other characters when idle
          serialCmdInputBuffer = "";
          printInvalidInputWarning();
        }
        break;
        
      case SerialState::S1_GOT_T:
        if (c == 'v'|| c == 'V') {
          serialCmdInputBuffer += c;
          T_Display_S3.print(c); // Echo the character
          currentState = SerialState::S2_GOT_TV;
        } else {
          // Invalid sequence, reset
          serialCmdInputBuffer = "";
          currentState = SerialState::S0_IDLE;
          T_Display_S3.println("\n[Error: Expected 'v' after 't']");
        }
        break;

      case SerialState::S1_GOT_D:
        if (c == 'v' || c == 'V') {
          serialCmdInputBuffer += c;
          T_Display_S3.print(c); // Echo the character
          currentState = SerialState::S2_GOT_DV;
        } else {
          // Invalid sequence, reset
          serialCmdInputBuffer = "";
          currentState = SerialState::S0_IDLE;
          T_Display_S3.println("\n[Error: Expected 'v' after 'd']");
        }
        break;

      case SerialState::S2_GOT_DV:
        if (c == 'f' || c == 'F') {
          serialCmdInputBuffer += c;
          T_Display_S3.print(c); // Echo the character
          // Execute display flip
          uint8_t rot = T_Display_S3.getRotation();
          T_Display_S3.setRotation((rot + 2) % 4);
          T_Display_S3.clear();
          T_Display_S3.println("\n[Display flipped]");
          // Reset state
          serialCmdInputBuffer = "";
          currentState = SerialState::S0_IDLE;
        } else if (c == '\n' || c == '\r') {
          // End without 'f'
          serialCmdInputBuffer = "";
          currentState = SerialState::S0_IDLE;
          T_Display_S3.println("\n[Error: Expected 'f' after 'dv']");
        } else {
          // Invalid character
          serialCmdInputBuffer = "";
          currentState = SerialState::S0_IDLE;
          T_Display_S3.println("\n[Error: Expected 'f' after 'dv']");
        }
        break;

      case SerialState::S1_GOT_C:
        if (c == 'l' || c == 'L') {
          serialCmdInputBuffer += c;
          T_Display_S3.print(c); // Echo the character
          // Execute display clear
          T_Display_S3.clear();
          T_Display_S3.println("\n[Display cleared]");
          // Reset state
          serialCmdInputBuffer = "";
          currentState = SerialState::S0_IDLE;
        } else if (c == '\n' || c == '\r') {
          // End without 'l'
          serialCmdInputBuffer = "";
          currentState = SerialState::S0_IDLE;
          T_Display_S3.println("\n[Error: Expected 'l' after 'c']");
        } else {
          // Invalid character
          serialCmdInputBuffer = "";
          currentState = SerialState::S0_IDLE;
          T_Display_S3.println("\n[Error: Expected 'l' after 'c']");
        }
        break;
        
      case SerialState::S2_GOT_TV:
        if (c >= '0' && c <= '9') {
          // Check for leading zero (buffer should have more than just "tv")
          if (serialCmdInputBuffer.length() == 3 && serialCmdInputBuffer[2] == '0'&& c == '0') {
            // Reject leading zero
            serialCmdInputBuffer = "";
            currentState = SerialState::S0_IDLE;
            T_Display_S3.println("\n[Error: Leading zero not allowed]");
          } else {
            // Valid digit
            if (serialCmdInputBuffer.length() < MAX_BUFFER_SIZE - 1) {
              serialCmdInputBuffer += c;
              T_Display_S3.print(c); // Echo the digit
            } else {
              // Buffer full, reset
              serialCmdInputBuffer = "";
              currentState = SerialState::S0_IDLE;
              T_Display_S3.println("\n[Error: Input too long]");
            }
          }
        } else if (c == '.' || c == '-') {
          // Allow decimal point and negative sign for float parsing
          if (serialCmdInputBuffer.length() < MAX_BUFFER_SIZE - 1) {
            serialCmdInputBuffer += c;
            T_Display_S3.print(c); // Echo the character
          } else {
            serialCmdInputBuffer = "";
            currentState = SerialState::S0_IDLE;
            T_Display_S3.println("\n[Error: Input too long]");
          }
        } else if (c == '\n' || c == '\r') {
          // End of command
          if (serialCmdInputBuffer.length() > 2) {
            // Extract velocity value (skip "tv")
            String velocityStr = serialCmdInputBuffer.substring(2);
            float requested_velocity = velocityStr.toFloat();
            
            // Call the velocity setting function
            writeTargetVelocityLocked(requested_velocity);
            
            T_Display_S3.print("\ntv=");
            T_Display_S3.println(requested_velocity);
          } else {
            T_Display_S3.println("\n[Error: No velocity value provided]");
          }
          // Reset state
          serialCmdInputBuffer = "";
          currentState = SerialState::S0_IDLE;
        } else {
          // Invalid character, reset
          serialCmdInputBuffer = "";
          currentState = SerialState::S0_IDLE;
          T_Display_S3.println("\n[Error: Invalid character]");
        }
        break;
    }
  }
}
