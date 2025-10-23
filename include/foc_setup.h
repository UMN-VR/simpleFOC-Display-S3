#ifndef FOC_SETUP_H
#define FOC_SETUP_H

// Initialize FOC motor and driver
void setupFOC();

// Wait for FOC to be ready (for core 1)
void waitForFOCReady();

// Check if FOC is ready
bool isFOCReady();

#endif // FOC_SETUP_H
