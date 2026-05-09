#ifndef TIMERS_H
#define TIMERS_H

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

extern volatile uint32_t reaction_ticks;
extern volatile uint8_t timer1_running;

void Timer1_init(void);
void Timer1_start(void);
void Timer1_stop(void);
uint32_t Timer1_get_ms(void);

#endif