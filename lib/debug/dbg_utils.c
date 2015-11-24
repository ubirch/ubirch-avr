/**
 * debug helpers
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

#include "dbg_utils.h"
#include <avr/io.h>
#include <util/delay.h>

// create a string with the bits contained in this byte (not thread safe!)
void print_bits(size_t size, void const *const ptr) {
    unsigned char *b = (unsigned char *) ptr;
    unsigned char byte;
    int i, j;

    for (i = size - 1; i >= 0; i--) {
        for (j = 7; j >= 0; j--) {
            byte = (unsigned char) (b[i] & (1 << j));
            byte >>= j;
            printf("%u", byte);
        }
    }
}

void blink(uint8_t n) {// blink a few times before we start
    DDRB |= _BV(PINB5);
    if(n <= 0) return;
    while(--n) {
        printf(".");
        PORTB ^= _BV(PORTB5);
        _delay_ms(3000);
    }
}
