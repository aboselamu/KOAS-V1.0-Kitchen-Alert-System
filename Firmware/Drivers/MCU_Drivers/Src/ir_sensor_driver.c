#include "ir_sensor_driver.h"
#include "stm32f1xx.h"

/*
 * HUAXING NPN normally-ON sensor via 2.2kΩ/10kΩ voltage divider:
 *
 *   Beam CLEAR (no ticket) : sensor HIGH → PA1 HIGH → IDR bit 1 = 1
 *   Beam BROKEN (ticket)   : sensor LOW  → PA1 LOW  → IDR bit 1 = 0
 *
 * Therefore: IsBeamBroken returns true when PA1 is LOW (bit = 0)
 */
bool IR_Sensor_IsBeamBroken(void)
{
    return ((GPIOA->IDR & (1U << 1)) == 0U);   /* LOW = beam broken */
}
