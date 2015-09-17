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



#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <uart_stdio.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-stack-address"
#pragma clang diagnostic ignored "-Wmissing-noreturn"

int main() {
    UART_INIT_STDIO();

    // disable watchdog
    DDRD |= _BV(PIND6);
    DDRB &= ~_BV(PINB1);

    uart_init();

    printf("START\n");

    int i = 0;
    while (1) {
        PORTB &= ~_BV(PORTB5);
        uint8_t v = PINB & _BV(PINB1);
        if (!v) {
            i++;
            for (uint8_t j = 0; j < i; j++) printf("=\x1b[K");
            PORTB |= _BV(PORTB5);
        } else {
            if (i > 1) printf("\n");
            i = 0;
        }
        _delay_ms(500);
    }
}

#pragma clang diagnostic pop
