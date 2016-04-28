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
#include <isl29125.h>

#include <math.h>
#include <dbg_utils.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

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

    i2c_init(I2C_SPEED_50KHZ);

    // reset device
    if (!isl_reset()) {
        puts("could not initialize ISL29125 RGB sensor");
    }

    // set sampling mode, ir filter and interrupt mode
    isl_set(ISL_R_COLOR_MODE, ISL_MODE_RGB | ISL_MODE_10KLUX | ISL_MODE_12BIT);
    isl_set(ISL_R_FILTERING, ISL_FILTER_IR_MAX);
    isl_set(ISL_R_INTERRUPT, ISL_INT_ON_THRSLD);

    printf("read mode: ");
    uint8_t color_mode = isl_get(ISL_R_COLOR_MODE);
    print_bits(1, &color_mode);
    puts("");
    printf("read filter: ");
    uint8_t ir_filtering = isl_get(ISL_R_FILTERING);
    print_bits(1, &ir_filtering);
    puts("");
    printf("read interrupts: ");
    uint8_t intr = isl_get(ISL_R_INTERRUPT);
    print_bits(1, &intr);
    puts("");

    puts("reading RGB values from sensor");
    puts("'%' indicates the chip is still in a conversion cyle, so we wait");
    while (1) {
        // wait for the conversion cycle to be done, this just indicates there is a cycle
        // in progress. the actual r,g,b values are always available from the last cycle
        for (uint8_t colors = 0; colors < 3; colors++)
            while (!(isl_get(ISL_R_STATUS) & ISL_STATUS_ADC_DONE))
                putchar('%');
        puts("");

        // read the full 36 or 48 bit color
        printf("48bit: ");
        rgb48 rgb = isl_read_rgb();
        printf("0x%04x%04x%04x rgb48(%u,%u,%u)\n", rgb.green, rgb.red, rgb.blue, rgb.red, rgb.green, rgb.blue);

        printf("24bit: ");
        rgb24 rgb8 = isl_read_rgb24();
        printf("0x%02x%02x%02x rgb24(%u,%u,%u)\n", rgb8.red, rgb8.green, rgb8.blue, rgb8.red, rgb8.green, rgb8.blue);

        // here we convert them as in the FEWL sensor
        printf("old  : ");
        unsigned int red_old_x = (unsigned int) sqrt(sqrt(rgb.red * rgb.red / 1));
        unsigned int green_old_x = (unsigned int) sqrt(sqrt(rgb.green * rgb.green / 1));
        unsigned int blue_old_x = (unsigned int) sqrt(sqrt(rgb.blue * rgb.blue / 1));
        printf("0x%02x%02x%02x rgb24(%u,%u,%u)\n", red_old_x, green_old_x, blue_old_x, red_old_x, green_old_x,
               blue_old_x);

        prompt("next? ");
    }
}

#pragma clang diagnostic pop