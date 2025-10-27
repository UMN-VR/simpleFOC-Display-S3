#ifndef FOC_SETUP_H
#define FOC_SETUP_H

enum class FOCMode {
	OpenLoop,
	ClosedLoop,
	Position,
};

// Shared status struct for RT core to background core communication
struct FOCStatus {
    bool driverInitSuccess = false;
    bool motorInitSuccess = false;
    FOCMode activeMode = FOCMode::OpenLoop;
    bool sensorAvailable = false;
    bool focReady = false;
};

extern FOCStatus focStatus;

// Configure the desired FOC mode prior to calling setupFOC().
void setFOCMode(FOCMode mode);

// Retrieve the mode that setupFOC() will attempt to activate.
FOCMode getFOCMode();

// Initialize FOC motor and driver using the currently configured mode.
void setupFOC();

// RT core setup function
void setupFOC_RT();

// Enable the motor (called from RT task)
void enableMotor();

// Print AS5600 angle telemetry roughly once per second when available.
void logEncoderAngleOncePerSecond();

// Wait for FOC to be ready (for core 1)
void waitForFOCReady();

// Check if FOC is ready
bool isFOCReady();

// Log setup status (for background core)
void logSetupStatus(FOCMode activeMode, bool sensorAvailable);

#endif // FOC_SETUP_H
