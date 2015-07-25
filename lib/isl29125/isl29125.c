/**
 * RGB sensor library (ISL29125)
 *
 * The library assumes that the i2c bus has been initialized already.
 * It will only communicate by issuing start/stop conditions and transmitting
 * command and data requests.
 *
 * The 36 bit color mode is supported. However, downsampling it to 24 bit will
 * incur an extra cost of reading the color mode register (i2c transmission) for
 * each color.
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

#include "isl29125.h"

// disable asserts in non-debug code
#include <i2c.h>
#include <math.h>

#ifndef NDEBUG
#   ifdef i2c_assert
#      undef i2c_assert
#   endif
#   define i2c_assert(e, m)
#endif

uint8_t isl_read(uint8_t reg) {
    i2c_start();
    i2c_write(ISL_DEVICE_ADDRESS << 1);
    i2c_assert(I2C_STATUS_SLAW_ACK, "address error");
    i2c_write(reg);
    i2c_assert(I2C_STATUS_DATA_ACK, "device-id error");

    i2c_start();
    i2c_write((ISL_DEVICE_ADDRESS << 1) | 0x01);
    i2c_assert(I2C_STATUS_SLAR_ACK, "address error");
    uint8_t r = i2c_read(false);
    i2c_assert(I2C_STATUS_RCVD_DATA_NACK, "data receive error");
    i2c_stop();

    return r;
}

uint16_t isl_read16(uint8_t reg) {
    i2c_start();
    i2c_write(ISL_DEVICE_ADDRESS << 1);
    i2c_assert(I2C_STATUS_SLAW_ACK, "address error");
    i2c_write(reg);
    i2c_assert(I2C_STATUS_DATA_ACK, "device-id error");

    i2c_start();
    i2c_write((ISL_DEVICE_ADDRESS << 1) | 0x01);
    i2c_assert(I2C_STATUS_SLAR_ACK, "address error");
    uint16_t r = i2c_read(true) | (i2c_read(false) << 8);
    i2c_assert(I2C_STATUS_RCVD_DATA_NACK, "data receive error");
    i2c_stop();

    return r;
}

void isl_set(uint8_t reg, uint8_t data) {
    i2c_start();
    i2c_write(ISL_DEVICE_ADDRESS << 1);
    i2c_assert(I2C_STATUS_SLAW_ACK, "address error");
    i2c_write(reg);
    i2c_assert(I2C_STATUS_DATA_ACK, "register error");
    i2c_write(data);
    i2c_assert(I2C_STATUS_DATA_ACK, "value error");
    i2c_stop();
}

uint8_t isl_get(uint8_t reg) {
    return isl_read(reg);
}

uint8_t isl_reset(void) {
    i2c_start();
    i2c_write(ISL_DEVICE_ADDRESS << 1);
    i2c_assert(I2C_STATUS_SLAW_ACK, "address error");
    i2c_write(ISL_R_RESET);
    i2c_assert(I2C_STATUS_DATA_ACK, "register error");
    i2c_stop();

    uint8_t id = isl_read(ISL_R_DEVICE_ID);
    if (id != ISL_R_DEVICE_ID) return 0;

    uint8_t check = 0x00;
    check |= isl_read(ISL_R_COLOR_MODE);
    check |= isl_read(ISL_R_FILTERING);
    check |= isl_read(ISL_R_INTERRUPT);
    check |= isl_read(ISL_R_STATUS);
    if (check) return 0;

    return 1;
}

uint16_t isl_read_red(void) {
    return isl_read16(ISL_R_RED_L);
}

uint16_t isl_read_green(void) {
    return isl_read16(ISL_R_GREEN_L);
}

uint16_t isl_read_blue(void) {
    return isl_read16(ISL_R_BLUE_L);
}

static uint8_t downsample(uint16_t c, double gamma) {
    double range = (isl_read(ISL_R_COLOR_MODE) & ISL_MODE_12BIT) ? 4096.0 : 65535.0;
    double sample = (255 * pow(c / range, 1.0 / (gamma > 0 ? gamma : 1)));
    return (uint8_t) sample;
}

uint8_t isl_read_red8(double gamma) {
    return downsample(isl_read_red(), gamma);
}

uint8_t isl_read_green8(double gamma) {
    return downsample(isl_read_green(), gamma);;
}

uint8_t isl_read_blue8(double gamma) {
    return downsample(isl_read_blue(), gamma);
}

rgb48 isl_read_rgb(void) {
    static rgb48 result;
    result.red = isl_read_red();
    result.blue = isl_read_blue();
    result.green = isl_read_green();
    return result;
}

rgb24 isl_read_rgb24(double gamma) {
    rgb24 result;
    result.red = isl_read_red8(gamma);
    result.green = isl_read_green8(gamma);
    result.blue = isl_read_blue8(gamma);
    return result;
}
