#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "uptime.h"
#include "usart.h"
#include "twi.h"
#include "ssd1306.h"

int main(void)
{
    // PB0, PB1, PB2 countdown
    DDRB |= (1 << PB0);
    DDRB |= (1 << PB1);
    DDRB |= (1 << PB2);

    // PB7 start game
    DDRB &= ~(1 << PB7);
    PORTB |= (1 << PB7);

    // PD2 Player 1
    DDRD &= ~(1 << PD2);
    PORTD |= (1 << PD2);

    // PD3 Player 2
    DDRD &= ~(1 << PD3);
    PORTD |= (1 << PD3);

    uptime_init();
    USART0_init(9600);
    twi_init();
    ssd1306_init();
    ssd1306_clear();
    ssd1306_print_str("READY", 0);
    twi_discover();
    sei();

    while (1) {
        // wait for button press
        while (PINB & (1 << PB7));
        // debounce
        _delay_ms(50);
        // wait for button release
        while (!(PINB & (1 << PB7)));
        // debounce
        _delay_ms(50);

        ssd1306_clear();

        // turn on LEDs one by one
        PORTB |= (1 << PB0);
        uint32_t t = uptime_ms();
        // wait 1s non-blocking
        while ((uptime_ms() - t) < 1000);
        
        PORTB |= (1 << PB1);
        t = uptime_ms();
        while ((uptime_ms() - t) < 1000);
        
        PORTB |= (1 << PB2);
        t = uptime_ms();
        while ((uptime_ms() - t) < 1000);

        // random delay 1-3 sec
        srand(uptime_ms());
        uint32_t random_delay = (rand() % 3 + 1) * 1000UL;
        t = uptime_ms();
        while ((uptime_ms() - t) < random_delay);

        // turn off LEDs
        PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2));
        uint32_t t_off = uptime_ms();

        uint8_t winner = 0;
        uint32_t reaction_time_p1 = 0;
        uint32_t reaction_time_p2 = 0;

        while (reaction_time_p1 == 0 || reaction_time_p2 == 0) {
            if (!(PIND & (1 << PD2)) && reaction_time_p1 == 0) {
                reaction_time_p1 = uptime_ms() - t_off;
                if (winner == 0) {
                    winner = 1;
                }
            }
            if (!(PIND & (1 << PD3)) && reaction_time_p2 == 0) {
                reaction_time_p2 = uptime_ms() - t_off;
                if (winner == 0) {
                    winner = 2;
                }
            }
            if ((uptime_ms() - t_off) > 5000) break;
        }

        char buf1[8], buf2[8];
        uint8_t j = 0;
        if (reaction_time_p1 >= 1000) {
            buf1[j++] = '0' + (reaction_time_p1 / 1000);
        }
        buf1[j++] = '0' + ((reaction_time_p1 % 1000) / 100);
        buf1[j++] = '0' + ((reaction_time_p1 % 100) / 10);
        buf1[j++] = '0' + (reaction_time_p1 % 10);
        buf1[j++] = 'M'; buf1[j++] = 'S'; buf1[j] = '\0';

        j = 0;
        if (reaction_time_p2 >= 1000) {
            buf2[j++] = '0' + (reaction_time_p2 / 1000);
        }
        buf2[j++] = '0' + ((reaction_time_p2 % 1000) / 100);
        buf2[j++] = '0' + ((reaction_time_p2 % 100) / 10);
        buf2[j++] = '0' + (reaction_time_p2 % 10);
        buf2[j++] = 'M';
        buf2[j++] = 'S';
        buf2[j] = '\0';

        if (winner == 1) {
            ssd1306_print_str("P1 WINS", 0);
        }
        else {
            ssd1306_print_str("P2 WINS", 0);
        }
        ssd1306_print_str("P1:", 2);
        ssd1306_print_str(buf1, 3);
        ssd1306_print_str("P2:", 5);
        ssd1306_print_str(buf2, 6);
    }
}