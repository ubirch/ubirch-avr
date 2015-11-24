/**
 * i2c tilt sensor
 *
 * @author Matthias L. Jugel
 *
 * == LICENSE ==
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

#include "uart.h"
#include "uart_stdio.h"
#include "dbg_utils.h"

#include <util/delay.h>
#include <avr/io.h>
#include <avr/sleep.h>

const uint8_t rows[8][2] = {
        0x05, PB0,
        0x05, PB1,
        0x05, PB2,
        0x05, PB3,
        0x05, PB4,
        0x05, PB5,
        0x0B, PD6,
        0x0B, PD7
};
const uint8_t cols[8][2] = {
        0x08, PC0,
        0x08, PC1,
        0x08, PC2,
        0x08, PC3,
        0x08, PC4,
        0x08, PC5,
        0x0B, PD4,
        0x0B, PD5
};

uint8_t screen[8] = {
        0b10000000,
        0b01000000,
        0b00100000,
        0b00010000,
        0b00001000,
        0b00000100,
        0b00000010,
        0b00000001
};


void init_screen() {
    for (uint8_t r = 0; r < 8; r++) {
        _SFR_IO8(rows[r][0] - 1) |= _BV(rows[r][1]);
        _SFR_IO8(rows[r][0]) &= ~_BV(rows[r][1]);
        for (uint8_t c = 0; c < 8; c++) {
            _SFR_IO8(cols[c][0] - 1) |= _BV(cols[c][1]);
            _SFR_IO8(cols[c][0]) |= _BV(cols[c][1]);
        }
    }
}


void refresh_screen(uint8_t *buffer, uint8_t delay) {
    for (uint8_t r = 0; r < 8; r++) {
        _SFR_IO8(rows[r][0]) &= ~_BV(rows[r][1]);
        for (uint8_t c = 0; c < 8; c++) {
            if (buffer[r] & _BV(c)) {
                _SFR_IO8(cols[c][0]) &= ~_BV(cols[c][1]);
            } else {
                _SFR_IO8(cols[c][0]) |= ~_BV(cols[c][1]);
            }
        }
        _SFR_IO8(rows[r][0]) |= _BV(r);
    }
}

void set_pixel(uint8_t x, uint8_t y) {
    _SFR_IO8(rows[y][0]) |= _BV(rows[y][1]);
    _SFR_IO8(cols[x][0]) &= ~_BV(cols[x][1]);
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#pragma clang diagnostic ignored "-Wreturn-stack-address"

int main(void) {
    UART_INIT_STDIO();

    init_screen();
    set_pixel(2, 5);
    set_pixel(2, 3);

    set_pixel(4, 2);
    set_pixel(4, 5);

//    while(1) {
//        refresh_screen(screen, 100);
//        _delay_ms(3000);
//        refresh_screen(screen, 10);
//        _delay_ms(3000);
//        refresh_screen(screen, 1);
//        _delay_ms(3000);
//    }

    while (1) {
        sleep_cpu();
    };
}

#pragma clang diagnostic pop