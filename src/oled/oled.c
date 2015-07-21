/**
 * oled example
 *
 * data sheet for the controller:
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

void status(uint8_t expected, char *msg) {
    uint8_t s = i2c_status();
    if (s != expected) {
        printf("i2c: status: 0x%02x (expected 0x%02x): %s\n", s, expected, msg);
    }
}

void reset(void) {
    // Reset the display
    DDRB |= _BV(PINB1);
    printf("DDRB=%s\n", bits(DDRB));
    PORTB |= _BV(PORTB1); // RST Pin LOW
    printf("PORTB=%s\n", bits(PORTB));
    _delay_ms(10);
    PORTB &= ~_BV(PORTB1); // RST Pin HIGH
    printf("PORTB=%s\n", bits(PORTB));
    _delay_ms(10);
    PORTB |= _BV(PORTB1); // RST pin LOW
    printf("PORTB=%s\n", bits(PORTB));
}

void write(uint8_t address, uint8_t reg, uint8_t data) {
    i2c_start();
    i2c_write(address << 1);
    status(I2C_STATUS_SLAW_ACK, "address error");
    i2c_write(reg);
    status(I2C_STATUS_DATA_ACK, "register error");
    i2c_write(data);
    status(I2C_STATUS_DATA_ACK, "value error");
    i2c_stop();
}

void writen(uint8_t address, uint8_t reg, uint8_t data[], uint8_t len) {
    i2c_start();
    i2c_write(address << 1);
    status(I2C_STATUS_SLAW_ACK, "address error");
    for (uint8_t i = 0; i < len; i++) {
        printf("sending 0x%02x\n", data[i]);
        i2c_write(reg);
        status(I2C_STATUS_DATA_ACK, "register error");
        i2c_write(data[i]);
        status(I2C_STATUS_DATA_ACK, "value error");
    }
    i2c_stop();
}

/**
 * I2C read a byte.
 */
uint8_t read(uint8_t address, uint8_t reg) {
    i2c_start();
    i2c_write(address << 1);
    status(I2C_STATUS_SLAW_ACK, "address error");
    i2c_write(reg);
    status(I2C_STATUS_DATA_ACK, "device-id error");

    i2c_start();
    i2c_write((address << 1) | 0x01);
    status(I2C_STATUS_SLAR_ACK, "address error");
    uint8_t r = i2c_read(false);
    status(I2C_STATUS_RCVD_DATA_NACK, "data receive error");
    i2c_stop();

    return r;
}


/**
 * I2C read an int (16bit)
 */
uint16_t read16(uint8_t address, uint8_t reg) {
    i2c_start();
    i2c_write(address << 1);
    status(I2C_STATUS_SLAW_ACK, "address error");
    i2c_write(reg);
    status(I2C_STATUS_DATA_ACK, "device-id error");

    i2c_start();
    i2c_write((address << 1) | 0x01);
    status(I2C_STATUS_SLAR_ACK, "address error");
    // ACK when more data expected, NACK for last byte
    uint16_t r = i2c_read(true);
    status(I2C_STATUS_RCVD_DATA_NACK, "data receive error");
    r |= (i2c_read(false) << 8);
    status(I2C_STATUS_RCVD_DATA_NACK, "data receive error");
    i2c_stop();

    return r;
}

void clear(void) {
    i2c_start();
    i2c_write(OLED_DEVICE_ADDRESS << 1);
    i2c_write(0x40);
    for (int i = 0; i < 1024; i++) i2c_write(0x00);
    i2c_stop();
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
    reset();

    i2c_init();

    _delay_ms(100); // wait for the display to come online

    // software configuration according to specs
    write(OLED_DEVICE_ADDRESS, 0x00, OLED_MULTIPLEX_RATIO);
    write(OLED_DEVICE_ADDRESS, 0x00, 0x2F);
    write(OLED_DEVICE_ADDRESS, 0x00, OLED_DISPLAY_OFFSET);
    write(OLED_DEVICE_ADDRESS, 0x00, 0x00);
    write(OLED_DEVICE_ADDRESS, 0x00, OLED_START_LINE | 0x00);
    write(OLED_DEVICE_ADDRESS, 0x00, OLED_SEGMENT_MAPPING1);
    write(OLED_DEVICE_ADDRESS, 0x00, OLED_SCAN_FLIPPED);
    write(OLED_DEVICE_ADDRESS, 0x00, OLED_COM_PIN_CONFIG);
    write(OLED_DEVICE_ADDRESS, 0x00, 0x12);
    write(OLED_DEVICE_ADDRESS, 0x00, OLED_CONTRAST);
    write(OLED_DEVICE_ADDRESS, 0x00, 0x7F);
    write(OLED_DEVICE_ADDRESS, 0x00, OLED_DISPLAY_RESUME);
    write(OLED_DEVICE_ADDRESS, 0x00, OLED_DISPLAY_NORMAL);
    write(OLED_DEVICE_ADDRESS, 0x00, OLED_CLOCK_DIV_FREQ);
    write(OLED_DEVICE_ADDRESS, 0x00, 0b100000); // 0x80: 1000 (freq) 00 (divider)
    write(OLED_DEVICE_ADDRESS, 0x00, OLED_CHARGE_PUMP);
    write(OLED_DEVICE_ADDRESS, 0x00, 0x14);

    write(OLED_DEVICE_ADDRESS, 0x00, OLED_ADDRESSING_MODE);
    write(OLED_DEVICE_ADDRESS, 0x00, OLED_ADDR_MODE_HORIZ);

    write(OLED_DEVICE_ADDRESS, 0x00, OLED_DISPLAY_ON);

    write(OLED_DEVICE_ADDRESS, 0x00, 0x21);
    write(OLED_DEVICE_ADDRESS, 0x00, 0);
    write(OLED_DEVICE_ADDRESS, 0x00, 128 - 1);

    write(OLED_DEVICE_ADDRESS, 0x00, 0x22);
    write(OLED_DEVICE_ADDRESS, 0x00, 0);
    write(OLED_DEVICE_ADDRESS, 0x00, 48 / 8 - 1);

    clear();

    i2c_start();
    i2c_write(OLED_DEVICE_ADDRESS << 1);
    status(I2C_STATUS_SLAW_ACK, "address error");
    i2c_write(0x40);
    status(I2C_STATUS_DATA_ACK, "reg error");

    for (int i = 0; i < 1024; i++) {
        i2c_write(0b10101010);
        status(I2C_STATUS_DATA_ACK, "address error");
        _delay_ms(10);
    }
    i2c_stop();

    prompt("invert? ");

//    clear(ALL);
//

    uint8_t state = OLED_DISPLAY_NORMAL;
    while (true) {
        if (state == OLED_DISPLAY_NORMAL) {
            write(OLED_DEVICE_ADDRESS, 0x00, OLED_DISPLAY_INVERSE);
            state = OLED_DISPLAY_INVERSE;
        } else {
            write(OLED_DEVICE_ADDRESS, 0x00, OLED_DISPLAY_NORMAL);
            state = OLED_DISPLAY_NORMAL;
        }
        _delay_ms(500);
    };

    while (1) {
        PORTB ^= _BV(PORTB5);
        _delay_ms(250);
    };


}

#pragma clang diagnostic pop