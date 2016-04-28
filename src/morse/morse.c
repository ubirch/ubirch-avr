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
#include <stdio.h>
#include <uart_stdio.h>
#include <util/delay.h>

#include "morse.h"

void adc_init() {
    // AREF = AVcc
    ADMUX = _BV(REFS0);
    // ENABLE ADC conversion + prescaler 128 => F_CPU (16M) / PRESCALER (128) = 125kHz
    ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
}

uint16_t adc_read(uint8_t ch) {
    ADMUX = (ADMUX & 0xF8) | (ch & 0b111); // clears the bottom 3 bits before ORing

    // start single convertion
    // write ’1′ to ADSC
    ADCSRA |= _BV(ADSC);

    // wait for conversion to complete
    // ADSC becomes ’0′ again
    // till then, run loop continuously
    while (ADCSRA & _BV(ADSC));

    return (ADC);
}

static inline void delay_ms(uint16_t count) {
    while (count--) _delay_ms(1);
}

static inline void delay_us(uint16_t count) {
    while (count--)_delay_us(1);
}

void pwm_sound(unsigned long frequency, unsigned int duration) {
    if (!frequency) {
        delay_ms(duration);
        return;
    } // Frequency is false (zero), nothing to play, just delay for duration and return.
    uint32_t notch = 500000UL / frequency;       // Calculate how fast to toggle the pin.
    uint32_t loops = (duration * 500UL) / notch; // Calculate the number of loops to run.

    DDRB |= _BV(PINB1);
    PORTB |= _BV(PORTB1);

    for (uint32_t i = 0; i < loops; i++) { // Loop for the desired number of loops.
        PORTB |= _BV(PORTB1);      // Set pin high.
        delay_us(notch);  // Square wave duration (how long to leave pin high).
        PORTB &= ~_BV(PORTB1);     // Set pin low.
        delay_us(notch);  // Square wave duration (how long to leave pin low).
    }
}

#define DOT_DURATION 1200/16

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#pragma clang diagnostic ignored "-Wreturn-stack-address"

int main(void) {
    UART_INIT_STDIO();

    // disable watchdog
    DDRD |= _BV(PIND6);

    DDRB |= _BV(PINB5);
    PORTB &= ~_BV(PORTB5);
    prompt("Press Enter to start...");
    printf("F_CPU=%lu, BAUD=%lu, MCU=%s\n", F_CPU, BAUD, MCU);

    static char text[] = "hello world this is ubirch 1 from danmark";

    while (1) {
        char *c = text;
        do {
            const char *code = M_CODE(*c);
            printf("%c: %s\n", *c, code);

            if (*c == ' ') {
                _delay_ms(DOT_DURATION * M_WORD_SEP);
            } else {
                do {
                    PORTB |= _BV(PORTB5);
                    pwm_sound(200, (const unsigned int) (DOT_DURATION * (*code == '-' ? 3 : 1)));
                    PORTB &= ~_BV(PORTB5);
                    _delay_ms(DOT_DURATION * M_SIGNAL_SEP);
                } while (*(++code));
                _delay_ms(DOT_DURATION * M_LETTER_SEP);
            }
        } while (*(++c));
        _delay_ms(DOT_DURATION * M_WORD_SEP * 2);
    }
}

#pragma clang diagnostic pop