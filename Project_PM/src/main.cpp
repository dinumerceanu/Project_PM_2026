#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "uptime.h"
#include "usart.h"
#include "twi.h"
#include "ssd1306.h"
#include "timers.h"

// false start flags
volatile uint8_t false_start_p1 = 0;
volatile uint8_t false_start_p2 = 0;
volatile uint8_t game_started = 0;

ISR(INT0_vect) {
    if (!game_started) {
        false_start_p1 = 1;
    }
}

ISR(INT1_vect) {
    if (!game_started) {
        false_start_p2 = 1;
    }
}

static void interrupts_init(void)
{
    // INT0 on PD2, falling edge
    EICRA |= (1 << ISC01);
    EICRA &= ~(1 << ISC00);
    // INT1 on PD3, falling edge
    EICRA |= (1 << ISC11);
    EICRA &= ~(1 << ISC10);
    // activate INT0 si INT1
    EIMSK |= (1 << INT0) | (1 << INT1);
}

int main(void)
{
    // PB0, PB1, PB2 countdown
    DDRB |= (1 << PB0);
    DDRB |= (1 << PB1);
    DDRB |= (1 << PB2);
    DDRB |= (1 << PB3);
    DDRB |= (1 << PB4);

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
    Timer1_init();
    USART0_init(9600);
    twi_init();
    interrupts_init();
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

        // reset false start flags
        false_start_p1 = 0;
        false_start_p2 = 0;
        game_started = 0;

        ssd1306_clear();
        PORTB &= ~((1 << PB3) | (1 << PB4));

        // turn on LEDs one by one
        PORTB |= (1 << PB0);
        uint32_t t = uptime_ms();
        // wait 1s non-blocking
        while ((uptime_ms() - t) < 1000 && !false_start_p1 && !false_start_p2);
        
        PORTB |= (1 << PB1);
        t = uptime_ms();
        while ((uptime_ms() - t) < 1000 && !false_start_p1 && !false_start_p2);
        
        PORTB |= (1 << PB2);
        t = uptime_ms();
        while ((uptime_ms() - t) < 1000 && !false_start_p1 && !false_start_p2);

        // random delay 1-3 sec
        srand(uptime_ms());
        uint32_t random_delay = (rand() % 3 + 1) * 1000UL;
        t = uptime_ms();
        while ((uptime_ms() - t) < random_delay && !false_start_p1 && !false_start_p2);

        // turn off LEDs
        PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2));
        Timer1_start();
        game_started = 1;

        uint8_t winner = 0;
        uint32_t reaction_time_p1 = 0;
        uint32_t reaction_time_p2 = 0;

        while ((reaction_time_p1 == 0 || reaction_time_p2 == 0) && !(false_start_p1 || false_start_p2)) {
            if (!(PIND & (1 << PD2)) && reaction_time_p1 == 0) {
                reaction_time_p1 = Timer1_get_ms();
                if (winner == 0) {
                    winner = 1;
                }
            }
            if (!(PIND & (1 << PD3)) && reaction_time_p2 == 0) {
                reaction_time_p2 = Timer1_get_ms();
                if (winner == 0) {
                    winner = 2;
                }
            }
            if (Timer1_get_ms() > 5000) break;
        }

        Timer1_stop();

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

        // display winner
        if (false_start_p1 && !false_start_p2) {
            ssd1306_print_str("P2 WINS", 0);
            ssd1306_print_str("P1 DISQ", 2);
            PORTB |= (1 << PB4);
            USART0_print("-------------------\r\n");
            USART0_print("P2 WINS\r\n");
            USART0_print("P1 DISQUALIFIED\r\n");
        } else if (false_start_p2 && !false_start_p1) {
            ssd1306_print_str("P1 WINS", 0);
            ssd1306_print_str("P2 DISQ", 2);
            PORTB |= (1 << PB3);
            USART0_print("-------------------\r\n");
            USART0_print("P1 WINS\r\n");
            USART0_print("P2 DISQUALIFIED\r\n");
        } else if (winner == 0) {
            ssd1306_print_str("NO WINNER", 0);
            ssd1306_print_str("TIMEOUT", 2);
            USART0_print("-------------------\r\n");
            USART0_print("NO WINNER - TIMEOUT\r\n");
        } else {
            // normal win
            if (winner == 1) {
                ssd1306_print_str("P1 WINS", 0);
                PORTB |= (1 << PB3);
            } else {
                ssd1306_print_str("P2 WINS", 0);
                PORTB |= (1 << PB4);
            }
            ssd1306_print_str("P1:", 2);
            ssd1306_print_str(buf1, 3);
            ssd1306_print_str("P2:", 5);
            ssd1306_print_str(buf2, 6);

            USART0_print("-------------------\r\n");

            if (winner == 1) {
                USART0_print("P1 WINS\r\n");
            } else {
                USART0_print("P2 WINS\r\n");
            }

            USART0_print("P1: ");
            USART0_print_u32(reaction_time_p1);
            USART0_print(" ms\r\n");

            USART0_print("P2: ");
            USART0_print_u32(reaction_time_p2);
            USART0_print(" ms\r\n");
        }
    }
}