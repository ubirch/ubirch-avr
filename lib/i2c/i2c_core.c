/**
 * i2c implementation core functionality.
 *
 * @author Matthias L. Jugel
 *
 * Copyright 2015 ubirch GmbH (http://www.ubirch.com)
 *
 * == LICENSE ==
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
#include <stdio.h>
#include "i2c_core.h"

void i2c_init(uint8_t speed) {
    DDRC |= _BV(PINC4) | _BV(PINC5);
    PORTC |= _BV(PINC4) | _BV(PINC5);

    // reset status register
    TWSR = 0x00;
    TWBR = speed;

    // reset TWI register
    TWCR = 0x00;
}

uint8_t i2c_status(void) {
    return TW_STATUS;
}

void i2c_start(void) {
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
    while (!(TWCR & _BV(TWINT)));
}

void i2c_stop(void) {
    TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
    while (TWCR & _BV(TWSTO));
}

void i2c_write(uint8_t b) {
    TWDR = b;
    TWCR = _BV(TWINT) | _BV(TWEN);
    while (!(TWCR & _BV(TWINT)));
}

uint8_t i2c_read(bool ack) {
    TWCR = _BV(TWINT) | _BV(TWEN);
    if (ack) TWCR |= _BV(TWEA);
    while (!(TWCR & _BV(TWINT)));
    return TWDR;
}

