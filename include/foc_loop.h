#ifndef FOC_LOOP_H
#define FOC_LOOP_H

// Run the FOC loop (check faults, read state, apply movement)
void runFOCBackgroundLoop();

// Run only the real-time FOC algorithm (loopFOC)
void runRealTimeFOCLoop();

#endif // FOC_LOOP_H
