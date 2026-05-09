#include "uptime.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

volatile uint32_t g_uptime_ms = 0;

ISR(TIMER2_COMPA_vect)
{
    g_uptime_ms++;
}

void uptime_init(void)
{
    /* Timer2 CTC mode */
    TCCR2A = (1 << WGM21);
    TCCR2B = 0;

    /*
        16MHz / 64 = 250000 Hz
        1ms = 250 ticks - 1 = 249
    */
    OCR2A = 249;

    TIMSK2 = (1 << OCIE2A);

    /* start timer with prescaler 64 */
    TCCR2B = (1 << CS22);
}

uint32_t uptime_ms(void)
{
    uint32_t t;
    uint8_t sreg = SREG;

    cli();
    t = g_uptime_ms;
    SREG = sreg;

    return t;
}