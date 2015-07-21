/**
 * rgb sensor example
 *
 * data sheet: http://www.intersil.com/content/dam/Intersil/documents/isl2/isl29125.pdf
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
#include "uart.h"
#include "uart_stdio.h"
#include "i2c.h"
#include "isl29125.h"
#include "dbg_utils.h"

#include <math.h>
#include <util/delay.h>

#define RGB_SENSOR_ADDRESS 0x44

bool status(uint8_t expected, const char *msg) {
    uint8_t s = i2c_status();
    if (s != expected) {
        printf("i2c: status: 0x%02x (expected 0x%02x): %s\n", s, expected, msg);
        return true;
    }
    return false;
}

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

void reset(uint8_t address) {
    i2c_start();
    i2c_write(address << 1);
    status(I2C_STATUS_SLAW_ACK, "address error");
    i2c_write(ISL_CMD_RESET);
    status(I2C_STATUS_DATA_ACK, "register error");
    i2c_stop();
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

    blink();
    prompt("press enter to start:");

    i2c_init();

    // read the device id register and check it
    uint8_t id = read(RGB_SENSOR_ADDRESS, ISL_R_DEVICE_ID);
    printf("receiving device id: 0x%02x (expect 0x%02x)\n", id, ISL_DEVICE_ID);

    // do the reset
    reset(RGB_SENSOR_ADDRESS);

    uint8_t check = 0x00;
    check |= read(RGB_SENSOR_ADDRESS, ISL_R_COLOR_MODE);
    check |= read(RGB_SENSOR_ADDRESS, ISL_R_FILTERING);
    check |= read(RGB_SENSOR_ADDRESS, ISL_R_INTERRUPT);
    check |= read(RGB_SENSOR_ADDRESS, ISL_R_STATUS);
    printf("reset result: 0x%02x (expect 0x00)\n", check);

    // set sampling mode
    uint8_t mode = ISL_MODE_RGB | ISL_MODE_10KLUX | ISL_MODE_16BIT;
    printf("setting mode: %s\n", bits(mode));
    write(RGB_SENSOR_ADDRESS, ISL_R_COLOR_MODE, mode);

    // set filtering mode
    uint8_t filtering = ISL_FILTER_IR_MAX;
    printf("setting filter: %s\n", bits(filtering));
    write(RGB_SENSOR_ADDRESS, ISL_R_FILTERING, filtering);

    // set interrupt mode
    uint8_t interrupts = ISL_INT_ON_THRSLD; // actually does nothing, sets all bits 0
    printf("setting interrupts: %s\n", bits(interrupts));
    write(RGB_SENSOR_ADDRESS, ISL_R_INTERRUPT, interrupts);

    printf("read mode: %s\n", bits(read(RGB_SENSOR_ADDRESS, ISL_R_COLOR_MODE)));
    printf("read filter: %s\n", bits(read(RGB_SENSOR_ADDRESS, ISL_R_FILTERING)));
    printf("read interrupts: %s\n", bits(read(RGB_SENSOR_ADDRESS, ISL_R_INTERRUPT)));

    puts("reading RGB values from sensor");
    puts("'%' indicates the chip is still in a conversion cyle, so we wait");
    while (1) {
        // wait for the conversion cycle to be done, this just indicates there is a cycle
        // in progress. the actual r,g,b values are always available from the last cycle
        while (!(read(RGB_SENSOR_ADDRESS, ISL_R_STATUS) & (ISL_STATUS_ADC_DONE))) putchar('%');
        puts("");

        // this is how the SparkFun code reads the RGB values (just the lower byte)
        printf("old  : ");
        unsigned int red_old = read16(RGB_SENSOR_ADDRESS, ISL_R_RED_L);
        unsigned int green_old = read16(RGB_SENSOR_ADDRESS, ISL_R_GREEN_L);
        unsigned int blue_old = read16(RGB_SENSOR_ADDRESS, ISL_R_BLUE_L);
        printf("0x%04x%04x%04x rgb24(%u,%u,%u)\n", red_old, green_old, blue_old, red_old, green_old, blue_old);

        // here we convert them as in the FEWL sensor
        printf("old  : ");
        unsigned int red_old_x = (unsigned int) sqrt(sqrt(red_old * red_old / 1));
        unsigned int green_old_x = (unsigned int) sqrt(sqrt(green_old * green_old / 1));
        unsigned int blue_old_x = (unsigned int) sqrt(sqrt(blue_old * blue_old / 1));
        printf("0x%02x%02x%02x rgb24(%u,%u,%u)\n", red_old_x, green_old_x, blue_old_x, red_old_x, green_old_x,
               blue_old_x);

        // read the full 36 or 48 bit color
        printf("48bit: ");
        uint16_t red = read(RGB_SENSOR_ADDRESS, ISL_R_RED_L) | (read16(RGB_SENSOR_ADDRESS, ISL_R_RED_H) << 8);
        uint16_t green = read(RGB_SENSOR_ADDRESS, ISL_R_GREEN_L) | (read16(RGB_SENSOR_ADDRESS, ISL_R_GREEN_H) << 8);
        uint16_t blue = read(RGB_SENSOR_ADDRESS, ISL_R_BLUE_L) | (read16(RGB_SENSOR_ADDRESS, ISL_R_BLUE_H) << 8);
        printf("0x%04x%04x%04x rgb48(%u,%u,%u)\n", red, green, blue, red, green, blue);

        printf("24bit: ");
        // adjust with gamma correction 2.2 (not scientific!)
        unsigned int red8 = (unsigned int) (255 * pow(red / 65535.0, 1 / 2.2));
        unsigned int green8 = (unsigned int) (255 * pow(green / 65535.0, 1 / 2.2));
        unsigned int blue8 = (unsigned int) (255 * pow(blue / 65535.0, 1 / 2.2));
        printf("0x%02x%02x%02x rgb24(%u,%u,%u)\n", red8, green8, blue8, red8, green8, blue8);
        prompt("next? ");
    }
}

#pragma clang diagnostic pop