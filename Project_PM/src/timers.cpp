#include "timers.h"

/* Reaction time counter in milliseconds */
volatile uint32_t reaction_ticks = 0;
volatile uint8_t timer1_running = 0;

/*
 * Initializes Timer1 in CTC mode for reaction time measurement.
 * Generates an interrupt every 1ms at 16MHz with prescaler 8.
 * OCR1A = 16MHz / (8 * 1000) - 1 = 1999
 */
void Timer1_init(void)
{
    /* Clear previous settings */
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;

    /* Set CTC mode */
    TCCR1B |= (1 << WGM12);

    /* Set prescaler to 8 */
    TCCR1B |= (1 << CS11);

    /* 16MHz / 8 = 2MHz => 1ms = 2000 ticks - 1 */
    OCR1A = 1999;

    /* Activate Compare A interrupt */
    TIMSK1 |= (1 << OCIE1A);
}

/*
 * Starts the reaction time counter by resetting it to 0.
 */
void Timer1_start(void)
{
    reaction_ticks = 0;
    timer1_running = 1;
}

/*
 * Stops the reaction time counter.
 */
void Timer1_stop(void)
{
    timer1_running = 0;
}

/*
 * Returns the elapsed time in milliseconds since Timer1_start() was called.
 */
uint32_t Timer1_get_ms(void)
{
    uint32_t t;
    uint8_t sreg = SREG;
    cli();
    t = reaction_ticks;
    SREG = sreg;
    return t;
}

ISR(TIMER1_COMPA_vect)
{
    if (timer1_running) {
        reaction_ticks++;
    }
}
