/**
 * i2c implementation
 *
 * @author Matthias L. Jugel
 *
 * Copyright 2015 ubirch GmbH (http://www.ubirch.com)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "i2c.h"

#ifdef DEBUG
#define DBG_MSG(...) printf(__VA_ARGS__)
#else
#define DBG_MSG(...)
#endif

void i2c_init(void) {
    DDRC |= _BV(PINC4) | _BV(PINC5);
    PORTC |= _BV(PINC4) | _BV(PINC5);

    // reset status register
    TWSR = 0x00;
    // set speed
    TWBR = ((((F_CPU / 400000UL) / 1) - 16) / 2);
    DBG_MSG("TWBR = 0x%02x\n", TWBR);
    // reset TWI register
    TWCR = 0x00;
}

uint8_t i2c_status(void) {
    return TW_STATUS;
}

void i2c_start(void) {
    DBG_MSG("->i2c_start()\n");
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
    while (!(TWCR & _BV(TWINT)));
    DBG_MSG("<-i2c_start(): 0x%02x\n", TW_STATUS);
}

void i2c_stop(void) {
    TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
}

void i2c_write(uint8_t b) {
    DBG_MSG("->i2c_write(0x%02x)\n", b);
    TWDR = b;
    TWCR = _BV(TWINT) | _BV(TWEN);
    while (!(TWCR & _BV(TWINT)));
    DBG_MSG("<-i2c_write(0x%02x): 0x%02x\n", b, TW_STATUS);
}

uint8_t i2c_read(bool ack) {
    TWCR = _BV(TWINT) | _BV(TWEN);
    if (ack) TWCR |= _BV(TWEN);
    while (!(TWCR & _BV(TWINT)));
    return TWDR;
}
