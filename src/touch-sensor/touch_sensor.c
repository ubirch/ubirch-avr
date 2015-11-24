/**
 * rgb sensor example
 *
 * data sheet: http://www.intersil.com/content/dam/Intersil/documents/isl2/isl29125.pdf
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

#include "i2c_core.h"
#include <mpr121.h>

#include <util/delay.h>
#include <dbg_utils.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#pragma clang diagnostic ignored "-Wreturn-stack-address"

int main(void) {
    // disable watchdog (pin 6 output)
    DDRD |= _BV(PIND6);
    // enable LED (pin 5 output)
    DDRB |= _BV(PINB5);

    // just in case we use I2C2 (second port)
    // enable power (pin 3 output + HIGH)
    DDRB |= _BV(PINB3);
    PORTB |= _BV(PORTB3);

    UART_INIT_STDIO();

    blink(3);
    prompt("press enter to start:");

    DDRB &= ~_BV(PINB5);

    i2c_init(I2C_SPEED_400KHZ);

    // reset device
    if (!mpr_reset()) {
        puts("could not initialize MPR121 touch sensor");
    }
    puts("MPR121 initialized");

    uint16_t lasttouched = 0;
    while (1) {
        uint16_t status = mpr_status();

        printf("\033[2J");
        for (uint8_t i = 0; i < 13; i++) {
            // it if *is* touched and *wasnt* touched before, alert!
            if ((status & _BV(i)) && !(lasttouched & _BV(i))) {
                printf("%d touched\n", i);
            }
            // if it *was* touched and now *isnt*, alert!
            if (!(status & _BV(i)) && (lasttouched & _BV(i))) {
                printf("%d released\n", i);
            }
        }

        // reset our state
        lasttouched = status;

        // debugging info, what
        printf("Tchd: ");
        print_bits(sizeof(status), (const void *const) &status);
        puts("");

        printf("Filt: ");
        uint8_t pin = 13;
        while (pin--) printf("%lu\t", (unsigned long) mpr_status_filtered(pin));
        puts("");

        printf("Base: ");
        pin = 13;
        while (pin--) printf("%lu\t", (unsigned long) mpr_baseline(pin));
        puts("");

        // put a delay so it isn't overwhelming
        _delay_ms(100);
    }

}

#pragma clang diagnostic pop