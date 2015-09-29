/*
 * ubirch #1 Eliza example code.
 *
 * @author Matthias L. Jugel
 *
 * == LICENSE ==
 * Copyright 2015 ubirch GmbH
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include "uart.h"
#include "eliza.h"

#include <string.h>
#include <avr/wdt.h>
#include <util/delay.h>

// set up the uart i/o as a stream, so wen can use it for stdio
FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

int main(void) {
    // disable watchdog (pin 6 output)
    DDRD = DDRD | 0b01000000;
//    wdt_enable(WDTO_8S);
    wdt_disable();

    uart_init();

    // this makes the standard io functions work on the serial port
    stdout = stdin = &uart_str;

    // wait a bit for the user to become ready
    for (uint8_t i = 0xff; i > 0; i--) {
        if (i % 4) putchar('.');
        _delay_ms(10);
    }
    printf("\n");

    char input[MAX_INPUT_BUFFER_SIZE];

    while (1) {
        print_generic_response();

        for (; ;) {
            // enable the LED
            PORTB |= _BV(PORTB5);

            // promt the user, read text from stdin and replace the newline with a \0
            printf(": ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0;

            // disable the LED
            PORTB ^= _BV(PORTB5);

            // if we read bye, lets exit (not really as we loop and start over)
            if (strncmp(input, "bye", MAX_INPUT_BUFFER_SIZE) == 0) {
                puts("Your bill will be mailed to you.");
                break;
            }

            // lets respond using the eliza functions
            respond(input);
        }
    }
}
#pragma clang diagnostic pop