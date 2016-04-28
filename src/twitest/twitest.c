/**
 * i2c scanner (goes through list of available addresses)
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
#include "dbg_utils.h"

#include <util/delay.h>


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#pragma clang diagnostic ignored "-Wreturn-stack-address"
int main(void) {
    UART_INIT_STDIO();

    // enable watchdog (pin 6 output)
    DDRD &= ~_BV(PIND6);

    // blink a few times before we start
    DDRB |= _BV(PINB5);
    for (uint8_t i = 3; i > 0; i--) {
        printf(".");
        PORTB ^= _BV(PORTB5);
        _delay_ms(3000);
    }

    // reset the oled display
    DDRB = _BV(PINB1);
    PORTB |= _BV(PORTB1);
    _delay_ms(5);
    PORTB &= ~_BV(PORTB1);
    _delay_ms(10);
    PORTB |= _BV(PORTB1);

    DDRB |= _BV(PINB3);
    PORTB |= _BV(PORTB3);

    puts("\n== Two Wire Interface Test");

    printf("Scanning for TWI (i2c) devices... (0x%02x - 0x%02x)\n", 0b00000000, 0b0111111);
    for (uint8_t address = 0b00000000; address < 0b01111111; address++) {
        printf("\r        \r0x%02x<", address);
        i2c_init(I2C_SPEED_50KHZ);
        i2c_start();
        printf("S");
        i2c_write(address << 1);
        printf("A");
        if (i2c_status() == I2C_STATUS_SLAW_ACK) {
            printf("\rFOUND DEVICE: 0x%02x 0b", address);
            print_bits(sizeof address, &address);
            printf("\n");
        }
        i2c_stop();
        printf(">");
    }
    puts("\nScanning done. Reset manually.");

    // do some low level blinking until the watchdog hits
    while (1) {
        PORTB ^= _BV(PORTB5);
        _delay_ms(250);
    };
}

#pragma clang diagnostic pop