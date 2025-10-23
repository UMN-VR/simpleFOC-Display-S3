#ifndef FOC_FAULT_H
#define FOC_FAULT_H

// Initialize fault detection (sets up pin mode)
void setupFaultDetection();

// Check and handle motor fault state
// Returns true if fault is currently active
bool checkAndHandleMotorFault();

#endif // FOC_FAULT_H
