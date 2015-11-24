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
#include "i2c_core.h"
#include "i2c_registers.h"
#include "dbg_utils.h"

#include <util/delay.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#pragma clang diagnostic ignored "-Wreturn-stack-address"

int main(void) {
    UART_INIT_STDIO();

    // enable watchdog (pin 6 output)
    DDRD &= ~_BV(PIND6);
    blink(3);

    printf("\n");
    i2c_init(I2C_SPEED_400KHZ);
    for (uint8_t i = 0x00; i < 0x3a; i++) {
        uint8_t value = i2c_read_reg(0x6b, i);
        printf("REG(0x%02x) = ", i);
        print_bits(sizeof value, &value);
        printf(" 0x%02x\n", value);
    }

    i2c_write_reg(0x6b, 0x39, 0b00000000);
    uint8_t ctrl1 = i2c_read_reg(0x6b, 0x20);
    i2c_write_reg(0x6b, 0x20, ctrl1 | 0b1010);

    // do some low level blinking until the watchdog hits
    while (1) {
        uint8_t status = i2c_read_reg(0x6b, 0x27);
        print_bits(sizeof status, &status);
        printf(" X(%d) Y(%d) Z(%d), REF(%02x) TEMP(%02dºC)\r",
               i2c_read_reg16(0x6b, 0x28),
               i2c_read_reg16(0x6b, 0x2A),
               i2c_read_reg16(0x6b, 0x3C),
               i2c_read_reg(0x6b, 0x25),
               i2c_read_reg(0x6b, 0x26)
        );

        PORTB ^= _BV(PORTB5);
        _delay_ms(50);
    };
}

#pragma clang diagnostic pop