#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>
#include <avr/io.h>

#define SSD1306_ADDR 0x3C

void ssd1306_init(void);
void ssd1306_clear(void);
void ssd1306_print_str(const char *str, uint8_t row);

#endif