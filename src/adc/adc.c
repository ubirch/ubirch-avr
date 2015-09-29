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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#pragma clang diagnostic ignored "-Wreturn-stack-address"

int main(void) {
    UART_INIT_STDIO();

    DDRB |= _BV(PINB5);
    PORTB &= _BV(PORTB5);

    uint16_t adc_result0;
    DDRC = _BV(PINC0);

    // initialize adc and lcd
    adc_init();

    _delay_ms(50);

    while (1) {
        adc_result0 = adc_read(0);      // read adc value at PA0

        // condition for led to glow
        if (adc_result0 > 500)
            PORTB |= _BV(PORTB5);
        else
            PORTB ^= _BV(PORTB5);

        // now display on lcd
        printf("ADC = %d\n", adc_result0);
//        _delay_ms(50);
    }
}

#pragma clang diagnostic pop