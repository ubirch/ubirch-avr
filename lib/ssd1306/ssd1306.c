/**
 * ...
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

#include <i2c.h>
#include <util/delay.h>
#include "ssd1306.h"

// == low level functions ==================================

void oled_reset(void) {
    DDRB |= _BV(PINB1);
    PORTB |= _BV(PORTB1);  // RST Pin LOW
    _delay_ms(10);
    PORTB &= ~_BV(PORTB1); // RST Pin HIGH
    _delay_ms(10);
    PORTB |= _BV(PORTB1);  // RST pin LOW
}


void oled_cmd(uint8_t address, uint8_t command) {
    i2c_write_reg(address, 0x00, command);
}


void oled_data(uint8_t address, uint8_t data) {
    i2c_write_reg(address, 0x40, data);
}

//== higher level functions ================================

void oled_clear(uint8_t address) {
    oled_cmd(address, 0x21);
    oled_cmd(address, 0);
    oled_cmd(address, 127);

    oled_cmd(address, 0x22);
    oled_cmd(address, 0);
    oled_cmd(address, 7);

    for (uint8_t page = 0; page < 8; page++) {
        oled_cmd(address, 0xb0 | page);
        oled_cmd(address, 0x00);
        i2c_start();
        i2c_write(address << 1);
        i2c_write(0x40);
        for (int i = 0; i < 128; i++) i2c_write(0x00);
        i2c_stop();
    }
}

