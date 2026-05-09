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

        // random delay 1-5 sec
        srand(uptime_ms());
        uint32_t random_delay = (rand() % 5 + 1) * 1000UL;
        t = uptime_ms();
        while ((uptime_ms() - t) < random_delay);

        // turn off LEDs
        PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2));

        USART0_print("LEDs off after: ");
        USART0_print_u32(random_delay);
        USART0_print(" ms\r\n");

        // print on I2C display
        ssd1306_clear();
        ssd1306_print_str("LEDS OFF AFTER:", 0);

        char buf[16];
        uint32_t ms = random_delay;

        // builds "XXXX MS" string
        uint8_t i = 0;
        if (ms >= 1000) {
            buf[i++] = '0' + (ms / 1000);
        }
        buf[i++] = '0' + ((ms % 1000) / 100);
        buf[i++] = '0' + ((ms % 100) / 10);
        buf[i++] = '0' + (ms % 10);
        buf[i++] = ' ';
        buf[i++] = 'M';
        buf[i++] = 'S';
        buf[i] = '\0';

        ssd1306_print_str(buf, 2);
    }
}