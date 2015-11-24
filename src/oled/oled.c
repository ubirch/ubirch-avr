/**
 * oled example + rgb display
 *
 * data sheet for this display:
 * http://www.buydisplay.com/download/manual/ER-OLED0.66-1_Series_Datasheet.pdf
 *
 * data sheet for the used controller:
 * https://www.adafruit.com/datasheets/SSD1306.pdf
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

#include <util/delay.h>
#include <uart_stdio.h>
#include <isl29125.h>
#include <dbg_utils.h>
#include <ssd1306.h>
#include <font5x5.h>
#include <avr/io.h>
#include "i2c_core.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#pragma clang diagnostic ignored "-Wreturn-stack-address"

int main(void) {
    UART_INIT_STDIO();

    // disable watchdog (pin 6 output)
    DDRD |= _BV(PIND6);
    DDRB |= _BV(PINB3);
    PORTB |= _BV(PORTB3);

//    blink(3);
//    prompt("press enter to start: ");

    i2c_init(I2C_SPEED_400KHZ);

    oled_reset();
    DBG_MSG("OLED RESET");

    // RGB sensor setup
    if (!isl_reset()) DBG_MSG("error: can't find rgb sensor");
    isl_set(ISL_R_COLOR_MODE, ISL_MODE_RGB | ISL_MODE_10KLUX | ISL_MODE_16BIT);
    isl_set(ISL_R_FILTERING, ISL_FILTER_IR_MAX);
    isl_set(ISL_R_INTERRUPT, ISL_INT_ON_THRSLD);

    _delay_ms(100); // wait for the display to come online

    // software configuration according to specs
    oled_cmd(OLED_DEVICE_ADDRESS, OLED_DISPLAY_OFF);
    oled_cmd(OLED_DEVICE_ADDRESS, OLED_CLOCK_DIV_FREQ);
    oled_cmd(OLED_DEVICE_ADDRESS, 0b100000); // 0x80: 1000 (freq) 00 (divider)
    oled_cmd(OLED_DEVICE_ADDRESS, OLED_MULTIPLEX_RATIO);
    oled_cmd(OLED_DEVICE_ADDRESS, 0x2F);
    oled_cmd(OLED_DEVICE_ADDRESS, OLED_DISPLAY_OFFSET);
    oled_cmd(OLED_DEVICE_ADDRESS, 0x00);
    oled_cmd(OLED_DEVICE_ADDRESS, OLED_START_LINE | 0x00);
    oled_cmd(OLED_DEVICE_ADDRESS, OLED_CHARGE_PUMP);
    oled_cmd(OLED_DEVICE_ADDRESS, 0x14);
    oled_cmd(OLED_DEVICE_ADDRESS, OLED_SCAN_REVERSE);
    oled_cmd(OLED_DEVICE_ADDRESS, OLED_SEGMENT_REMAP1);
    oled_cmd(OLED_DEVICE_ADDRESS, OLED_COM_PIN_CONFIG);
    oled_cmd(OLED_DEVICE_ADDRESS, 0x12);
    oled_cmd(OLED_DEVICE_ADDRESS, OLED_CONTRAST);
    oled_cmd(OLED_DEVICE_ADDRESS, 0x10);
    oled_cmd(OLED_DEVICE_ADDRESS, OLED_PRECHARGE_PERIOD);
    oled_cmd(OLED_DEVICE_ADDRESS, 0x22);
    oled_cmd(OLED_DEVICE_ADDRESS, OLED_VCOM_DESELECT);
    oled_cmd(OLED_DEVICE_ADDRESS, 0x00);
    oled_cmd(OLED_DEVICE_ADDRESS, OLED_DISPLAY_RESUME);
    oled_cmd(OLED_DEVICE_ADDRESS, OLED_DISPLAY_NORMAL);

    oled_cmd(OLED_DEVICE_ADDRESS, OLED_ADDRESSING_MODE);
    oled_cmd(OLED_DEVICE_ADDRESS, OLED_ADDR_MODE_PAGE);

    oled_clear(OLED_DEVICE_ADDRESS);

    oled_cmd(OLED_DEVICE_ADDRESS, OLED_DISPLAY_ON);

    // print the ABC character set (what fits) onto the display line 5
    oled_cmd(OLED_DEVICE_ADDRESS, OLED_PAGE_ADDR_START | 4);
    oled_cmd(OLED_DEVICE_ADDRESS, 0x00);
    oled_cmd(OLED_DEVICE_ADDRESS, 0x00);

    i2c_start();
    i2c_write(OLED_DEVICE_ADDRESS << 1);
    i2c_assert(I2C_STATUS_SLAW_ACK, "address error");
    i2c_write(0x40);
    i2c_assert(I2C_STATUS_DATA_ACK, "reg error");
    for (uint8_t i = 0; i < 128 - (128 / 6) - 1; i++) {
        if (i % 5 == 0) i2c_write(0);
        i2c_write(font5x5_abc[i]);
    }

    // print the extra character set (what fits) onto the display line 6
    oled_cmd(OLED_DEVICE_ADDRESS, OLED_PAGE_ADDR_START | 5);
    oled_cmd(OLED_DEVICE_ADDRESS, 0x00);
    oled_cmd(OLED_DEVICE_ADDRESS, 0x00);

    i2c_start();
    i2c_write(OLED_DEVICE_ADDRESS << 1);
    i2c_assert(I2C_STATUS_SLAW_ACK, "address error");
    i2c_write(0x40);
    i2c_assert(I2C_STATUS_DATA_ACK, "reg error");
    for (uint8_t i = 0; i < 128 - (128 / 6) - 1; i++) {
        if (i % 5 == 0) i2c_write(0);
        i2c_write(font5x5_extra[i]);
    }

    // configure scrolling of both lines 5 and 6
    oled_cmd(OLED_DEVICE_ADDRESS, OLED_SCROLL_LEFT);
    oled_cmd(OLED_DEVICE_ADDRESS, 0x00000000); // A dummy
    oled_cmd(OLED_DEVICE_ADDRESS, 0b00000100); // B start page
    oled_cmd(OLED_DEVICE_ADDRESS, 0b00000101); // C speed
    oled_cmd(OLED_DEVICE_ADDRESS, 0b00000111); // D end page
    oled_cmd(OLED_DEVICE_ADDRESS, 0b00000000); // E dummy
    oled_cmd(OLED_DEVICE_ADDRESS, 0b11111111); // F dummy
    oled_cmd(OLED_DEVICE_ADDRESS, OLED_SCROLL_START);

    // set drawing area (rows and pages)
    oled_cmd(OLED_DEVICE_ADDRESS, 0x21);
    oled_cmd(OLED_DEVICE_ADDRESS, 32);
    oled_cmd(OLED_DEVICE_ADDRESS, 64 + 32 - 1);

    oled_cmd(OLED_DEVICE_ADDRESS, 0x22);
    oled_cmd(OLED_DEVICE_ADDRESS, 0);
    oled_cmd(OLED_DEVICE_ADDRESS, 48 / 8 - 1);


    rgb24 rgb_cache = {.red=0, .green=0, .blue=0};
    while (true) {
        while (!(isl_get(ISL_R_STATUS) & ISL_STATUS_ADC_DONE)) continue;

        // read RGB values, convert the 0-255 into 0-64
        rgb24 rgb = isl_read_rgb24();
        // check if there was a change
        if (rgb_cache.red == rgb.red && rgb_cache.green == rgb.green && rgb_cache.blue == rgb.blue) {
            _delay_ms(100);
            continue;
        }
        rgb_cache.red = rgb.red;
        rgb_cache.green = rgb.green;
        rgb_cache.blue = rgb.blue;

        rgb48 rgbx = isl_read_rgb();
        uint8_t colors[6] = {
                rgb.red / 2,
                rgb.green / 2,
                rgb.blue / 2,
        };
        DBG_MSG("RGB48: %04x%04x%04x\n", rgbx.red, rgbx.green, rgbx.blue);
        DBG_MSG("RGB24: %02x%02x%02x\n", rgb.red, rgb.green, rgb.blue);


        // visualize the RGB levels using three gauges (stop scrolling to avoid picture erosion)
        for (uint8_t page = 0; page < 3; page += 1) {
            oled_cmd(OLED_DEVICE_ADDRESS, OLED_SCROLL_STOP);
            oled_cmd(OLED_DEVICE_ADDRESS, OLED_PAGE_ADDR_START | page);
            oled_cmd(OLED_DEVICE_ADDRESS, 0x12);
            oled_cmd(OLED_DEVICE_ADDRESS, 0x00);

            i2c_start();
            i2c_write(OLED_DEVICE_ADDRESS << 1);
            i2c_assert(I2C_STATUS_SLAW_ACK, "address error");
            i2c_write(0x40);
            i2c_assert(I2C_STATUS_DATA_ACK, "reg error");
            i2c_write(0b01111110);
            for (uint8_t column = 1; column < 63; column++) {
                if (colors[page] < column) i2c_write(0b01000010);
                else i2c_write(0b01011010);
                i2c_assert(I2C_STATUS_DATA_ACK, "address error");
            }
            i2c_write(0b01111110);
            i2c_stop();

            // now we can scroll again, we are waiting some time anyway
            oled_cmd(OLED_DEVICE_ADDRESS, OLED_SCROLL_START);

            _delay_ms(100);
        }
    }
}

#pragma clang diagnostic pop