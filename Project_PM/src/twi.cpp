#include "twi.h"

void twi_init(void) {
    /* Reset I2C control register */
    TWCR = 0;
    
    /* Set I2C clock frequency */
    TWBR = (uint8_t)TWBR_VAL;
    
    /* Set prescaler to 1 (TWPS1=0, TWPS0=0) */
    TWSR &= ~((1 << TWPS1) | (1 << TWPS0));
}

void twi_start(void) {
    /* Send START condition and wait for it to be transmitted */
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);
    while (!(TWCR & (1 << TWINT)));
}

void twi_write(uint8_t data) {
    /* Load data and trigger transmission with ACK enabled */
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    while (bit_is_clear(TWCR, TWINT));
}

void twi_read_ack(uint8_t *data) {
    /* Read a byte and send ACK to request another byte */
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    while (bit_is_clear(TWCR, TWINT));
    *data = TWDR;
}

void twi_read_nack(uint8_t *data) {
    /* Read a byte without ACK — signals last byte to slave */
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (bit_is_clear(TWCR, TWINT));
    *data = TWDR;
}

void twi_stop(void) {
    /* Send STOP condition */
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

void twi_discover(void) {
    /* Scan all I2C addresses and print those that respond */
    for (uint8_t i = 0x00; i < 0x7F; i++) {
        twi_start();
        twi_write((i << 1) | 1);
        if (TWSR == TW_MR_SLA_ACK) {
            USART0_print("Device found: 0x");
            USART0_print_u32(i);
            USART0_print("\r\n");
        }
    }
    twi_stop();
}
