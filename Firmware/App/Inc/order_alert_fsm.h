#ifndef ORDER_ALERT_FSM_H
#define ORDER_ALERT_FSM_H

#include <stdint.h>

/*
 * FSM States
 *
 * STATE_IDLE      : Beam clear, buzzer silent, waiting for order
 * STATE_ALARMING  : Beam broken, ticket present, buzzer pulsing
 * STATE_WAIT_ACK  : Ticket removed, buzzer still pulsing, waiting for button press
 */
typedef enum {
    STATE_IDLE     = 0,
    STATE_ALARMING = 1,
    STATE_WAIT_ACK = 2
} SystemState_t;

/*
 * FSM Events — delivered by driver callbacks in main.c
 *
 * TICKET_DETECTED : IR beam broken  (falling edge on PA1)
 * TICKET_REMOVED  : IR beam restored (rising edge on PA1)
 * BUTTON_PRESSED  : Silence button pressed (falling edge on PA0)
 */
typedef enum {
    TICKET_DETECTED = 0,
    TICKET_REMOVED  = 1,
    BUTTON_PRESSED  = 2
} SystemEvent_t;

/* Public API */
void          FSM_Init(void);
void          FSM_ProcessEvent(SystemEvent_t event);
SystemState_t FSM_GetState(void);   /* Used by main loop for pulse control */

#endif /* ORDER_ALERT_FSM_H */
