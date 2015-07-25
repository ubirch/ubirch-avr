/**
 * oled example
 *
 * data sheet for this display:
 * http://www.buydisplay.com/download/manual/ER-OLED0.66-1_Series_Datasheet.pdf
 *
 * data sheet for the used controller:
 * https://www.adafruit.com/datasheets/SSD1306.pdf
 *
 * @author Matthias L. Jugel
 *
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
#include "ssd1306.h"
#include "uart.h"
#include "i2c.h"

#include <util/delay.h>
#include <dbg_utils.h>
#include <uart_stdio.h>

static const uint8_t OLED_DEVICE_ADDRESS = 0x3d;

/**
 * A little prompt function to step through the code.
 */
void prompt(char *p) {
    printf(p);
    char input[10];
    fgets(input, sizeof(input), stdin);
}

/**
 * Blink a few times and output dots to show it's not crashed.
 */
void blink(void) {
    for (uint8_t i = 3; i > 0; i--) {
        putchar('.');
        PORTB ^= _BV(PORTB5);
        _delay_ms(3000);
    }
    puts("");
}

void isl_reset(void) {
    // Reset the display
    DDRB |= _BV(PINB1);
    PORTB |= _BV(PORTB1); // RST Pin LOW
    _delay_ms(10);
    PORTB &= ~_BV(PORTB1); // RST Pin HIGH
    _delay_ms(10);
    PORTB |= _BV(PORTB1); // RST pin LOW
}

void cmd(uint8_t data) {
    i2c_start();
    i2c_write(OLED_DEVICE_ADDRESS << 1);
    i2c_assert(I2C_STATUS_SLAW_ACK, "address error");
    i2c_write(0x00);
    i2c_assert(I2C_STATUS_DATA_ACK, "register error");
    i2c_write(data);
    i2c_assert(I2C_STATUS_DATA_ACK, "value error");
    i2c_stop();
}

void data(uint8_t data) {
    i2c_start();
    i2c_write(OLED_DEVICE_ADDRESS << 1);
    i2c_assert(I2C_STATUS_SLAW_ACK, "address error");
    i2c_write(0x40);
    i2c_assert(I2C_STATUS_DATA_ACK, "register error");
    i2c_write(data);
    i2c_assert(I2C_STATUS_DATA_ACK, "value error");
    i2c_stop();
}

/**
 * I2C read a byte.
 */
uint8_t read(uint8_t address, uint8_t reg) {
    i2c_start();
    i2c_write(address << 1);
    i2c_assert(I2C_STATUS_SLAW_ACK, "address error");
    i2c_write(reg);
    i2c_assert(I2C_STATUS_DATA_ACK, "device-id error");

    i2c_start();
    i2c_write((address << 1) | 0x01);
    i2c_assert(I2C_STATUS_SLAR_ACK, "address error");
    uint8_t r = i2c_read(false);
    i2c_assert(I2C_STATUS_RCVD_DATA_NACK, "data receive error");
    i2c_stop();

    return r;
}


/**
 * I2C read an int (16bit)
 */
uint16_t read16(uint8_t address, uint8_t reg) {
    i2c_start();
    i2c_write(address << 1);
    i2c_assert(I2C_STATUS_SLAW_ACK, "address error");
    i2c_write(reg);
    i2c_assert(I2C_STATUS_DATA_ACK, "device-id error");

    i2c_start();
    i2c_write((address << 1) | 0x01);
    i2c_assert(I2C_STATUS_SLAR_ACK, "address error");
    // ACK when more data expected, NACK for last byte
    uint16_t r = i2c_read(true);
    i2c_assert(I2C_STATUS_RCVD_DATA_NACK, "data receive error");
    r |= (i2c_read(false) << 8);
    i2c_assert(I2C_STATUS_RCVD_DATA_NACK, "data receive error");
    i2c_stop();

    return r;
}

void clear(void) {
    cmd(0x21);
    cmd(0);
    cmd(127);

    cmd(0x22);
    cmd(0);
    cmd(7);

    for (uint8_t page = 0; page < 8; page++) {
        cmd(0xb0 | page);
        cmd(0x00);
        i2c_start();
        i2c_write(OLED_DEVICE_ADDRESS << 1);
        i2c_write(0x40);
        for (int i = 0; i < 128; i++) i2c_write(0x00);
        i2c_stop();
    }
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

int main(void) {
    UART_INIT_STDIO();

    // disable watchdog (pin 6 output)
    DDRD |= _BV(PIND6);


//    blink();
//    prompt("press enter to start: ");

    // reset the device
    isl_reset();

    i2c_init(I2C_SPEED_400KHZ);

    _delay_ms(100); // wait for the display to come online

    // software configuration according to specs
    cmd(OLED_DISPLAY_OFF);
    cmd(OLED_CLOCK_DIV_FREQ);
    cmd(0b100000); // 0x80: 1000 (freq) 00 (divider)
    cmd(OLED_MULTIPLEX_RATIO);
    cmd(0x2F);
    cmd(OLED_DISPLAY_OFFSET);
    cmd(0x00);
    cmd(OLED_START_LINE | 0x00);
    cmd(OLED_CHARGE_PUMP);
    cmd(0x14);
    cmd(OLED_SCAN_REVERSE);
    cmd(OLED_SEGMENT_REMAP1);
    cmd(OLED_COM_PIN_CONFIG);
    cmd(0x12);
    cmd(OLED_CONTRAST);
    cmd(0xCF);
    cmd(OLED_PRECHARGE_PERIOD);
    cmd(0x22);
    cmd(OLED_VCOM_DESELECT);
    cmd(0x00);
    cmd(OLED_DISPLAY_RESUME);
    cmd(OLED_DISPLAY_NORMAL);

    cmd(OLED_ADDRESSING_MODE);
    cmd(OLED_ADDR_MODE_PAGE);

    clear();

    cmd(OLED_DISPLAY_ON);


    cmd(0x21);
    cmd(32);
    cmd(64 + 32 - 1);

    cmd(0x22);
    cmd(0);
    cmd(48 / 8 - 1);

    for (uint8_t page = 0; page < 3; page += 1) {
        cmd(OLED_PAGE_ADDR_START | page);
        cmd(0x12);
        cmd(0x00);

        i2c_start();
        i2c_write(OLED_DEVICE_ADDRESS << 1);
        i2c_assert(I2C_STATUS_SLAW_ACK, "address error");
        i2c_write(0x40);
        i2c_assert(I2C_STATUS_DATA_ACK, "reg error");
        i2c_write(0b11111111);
        for (uint8_t column = 1; column < 63; column++) {
            i2c_write(0b10111101);
            i2c_assert(I2C_STATUS_DATA_ACK, "address error");
        }
        i2c_write(0b11111111);
        i2c_stop();
    }

//    cmd(OLED_SCROLL_DIAG_RIGHT);
//    cmd(0x00);
//    cmd(0b00000111);
//    cmd(0b00000111);
//    cmd(0b00000111);
//    cmd(0b00000001);
//    cmd(OLED_SCROLL_START);

    prompt("?");

//    clear(ALL);
//

    uint8_t state = OLED_DISPLAY_NORMAL;
    uint8_t x = 0x00;
    while (true) {
        if (state == OLED_DISPLAY_NORMAL) {
            cmd(OLED_DISPLAY_INVERSE);
            state = OLED_DISPLAY_INVERSE;
        } else {
            cmd(OLED_DISPLAY_NORMAL);
            state = OLED_DISPLAY_NORMAL;
        }

        cmd(OLED_PAGE_ADDR_START | x++);
        cmd(0x12);
        cmd(0x00);

        _delay_ms(500);
    };

    while (1) {
        PORTB ^= _BV(PORTB5);
        _delay_ms(250);
    };


}

#pragma clang diagnostic pop