#include <avr/io.h>
#include "i2c.h"

void i2cInit() {
    TWBR0 = TWBR_VALUE;
    TWSR0 = ((1<<TWPS1)|(0<<TWPS0));
}

void i2cWait() {
    while (!(TWCR0 & (1 << TWINT)));
}

void i2cStart() {
    TWCR0 = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    i2cWait();
}

void i2cStop() {
    TWCR0 = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);

    while ((TWCR0 & (1 << TWSTO)));
}

void i2cSendData(uint8_t data) {
    TWDR0 = data;
    TWCR0 = (1 << TWINT) | (1 << TWEN);
    i2cWait();
}

uint8_t i2cReadDataAck() {
    TWCR0 = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    i2cWait();

    return TWDR0;
}

uint8_t i2cReadDataNotAck() {
    TWCR0 = (1 << TWINT) | (1 << TWEN);
    i2cWait();

    return TWDR0;
}

void i2cSendSlaveAddress(uint8_t address, uint8_t readWriteMode) {
    i2cSendData((address << 1) | readWriteMode);
}

uint8_t i2cGetStatus() {
    return TWSR0;
}

