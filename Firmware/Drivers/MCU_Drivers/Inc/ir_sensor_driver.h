#ifndef IR_SENSOR_DRIVER_H
#define IR_SENSOR_DRIVER_H

#include <stdbool.h>

/*
 * HUAXING IR Sensor — NPN normally-ON
 * Beam clear (no ticket) : sensor output HIGH → PA1 HIGH (after voltage divider)
 * Beam broken (ticket)   : sensor output LOW  → PA1 LOW
 */

/* Returns true if beam is broken (ticket present in window) */
bool IR_Sensor_IsBeamBroken(void);

#endif /* IR_SENSOR_DRIVER_H */
