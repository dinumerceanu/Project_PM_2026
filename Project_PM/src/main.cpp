#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    // PB0, PB1, PB2 countdown
    DDRB |= (1 << PB0);
    DDRB |= (1 << PB1);
    DDRB |= (1 << PB2);

    while (1) {
        PORTB |= (1 << PB0);
        _delay_ms(1000);
        PORTB |= (1 << PB1);
        _delay_ms(1000);
        PORTB |= (1 << PB2);
    }
}