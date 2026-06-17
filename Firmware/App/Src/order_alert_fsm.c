#include "order_alert_fsm.h"
#include "buzzer_driver.h"
#include "led_driver.h"

/*
 * FSM Transition Table:
 *
 *   STATE_IDLE     + TICKET_DETECTED  -> STATE_ALARMING  (Buzzer_On, LED_On)
 *   STATE_IDLE     + TICKET_REMOVED   -> STATE_IDLE      (ignored)
 *   STATE_IDLE     + BUTTON_PRESSED   -> STATE_IDLE      (ignored)
 *
 *   STATE_ALARMING + TICKET_DETECTED  -> STATE_ALARMING  (ignored — already alarming)
 *   STATE_ALARMING + TICKET_REMOVED   -> STATE_WAIT_ACK  (ticket gone, wait for ack)
 *   STATE_ALARMING + BUTTON_PRESSED   -> STATE_ALARMING  (ignored — ticket still present)
 *
 *   STATE_WAIT_ACK + TICKET_DETECTED  -> STATE_ALARMING  (new ticket dropped before ack)
 *   STATE_WAIT_ACK + TICKET_REMOVED   -> STATE_WAIT_ACK  (ignored)
 *   STATE_WAIT_ACK + BUTTON_PRESSED   -> STATE_IDLE      (Buzzer_Off, LED_Off)
 *
 * Design notes:
 *   - FSM never reads GPIO registers — fully hardware decoupled
 *   - Buzzer/LED pulse timing lives in main loop, not here
 *   - Buzzer stays ON when entering WAIT_ACK (main loop keeps pulsing)
 *   - Only BUTTON_PRESSED in WAIT_ACK silences the system
 */

/*===========================================================================
 * Private state
 *==========================================================================*/
static SystemState_t current_state = STATE_IDLE;

/*===========================================================================
 * Public API
 *==========================================================================*/
void FSM_Init(void)
{
    Buzzer_Off();
    LED_Off();
    current_state = STATE_IDLE;
}

void FSM_ProcessEvent(SystemEvent_t event)
{
    switch (current_state)
    {
        /*-------------------------------------------------------------------
         * IDLE: quiet, waiting for a ticket
         *-----------------------------------------------------------------*/
        case STATE_IDLE:
            if (event == TICKET_DETECTED)
            {
                Buzzer_On();
                LED_On();
                current_state = STATE_ALARMING;
            }
            /* TICKET_REMOVED and BUTTON_PRESSED ignored */
            break;

        /*-------------------------------------------------------------------
         * ALARMING: ticket present, beam broken, buzzer pulsing
         * Only ticket removal advances state — button has no effect here
         * because the ticket is still physically present
         *-----------------------------------------------------------------*/
        case STATE_ALARMING:
            if (event == TICKET_REMOVED)
            {
                /* Ticket lifted — keep buzzer on, wait for staff to press button */
                current_state = STATE_WAIT_ACK;
            }
            /* TICKET_DETECTED ignored (already alarming) */
            /* BUTTON_PRESSED ignored (ticket still present) */
            break;

        /*-------------------------------------------------------------------
         * WAIT_ACK: ticket removed, buzzer still pulsing, waiting for button
         *-----------------------------------------------------------------*/
        case STATE_WAIT_ACK:
            if (event == BUTTON_PRESSED)
            {
                /* Staff acknowledged — silence everything */
                Buzzer_Off();
                LED_Off();
                current_state = STATE_IDLE;
            }
            else if (event == TICKET_DETECTED)
            {
                /* New ticket dropped before staff pressed button
                 * Go back to ALARMING to catch the next removal */
                current_state = STATE_ALARMING;
            }
            /* TICKET_REMOVED ignored */
            break;

        /*-------------------------------------------------------------------
         * Safety net — should never reach here
         *-----------------------------------------------------------------*/
        default:
            Buzzer_Off();
            LED_Off();
            current_state = STATE_IDLE;
            break;
    }
}

SystemState_t FSM_GetState(void)
{
    return current_state;
}
