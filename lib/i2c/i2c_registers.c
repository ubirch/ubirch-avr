/**
 * I2C register level access (8 & 16 bit).
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

#include <stdio.h>
#include "i2c.h"

uint8_t i2c_read_reg(uint8_t addr, uint8_t reg) {
    i2c_start();
    i2c_write(addr << 1);
    i2c_assert(I2C_STATUS_SLAW_ACK, "address error");
    i2c_write(reg);
    i2c_assert(I2C_STATUS_DATA_ACK, "device-id error");

    i2c_start();
    i2c_write((addr << 1) | 0x01);
    i2c_assert(I2C_STATUS_SLAR_ACK, "address error");
    uint8_t r = i2c_read(false);
    i2c_assert(I2C_STATUS_RCVD_DATA_NACK, "data receive error");
    i2c_stop();

    return r;
}

uint16_t i2c_read_reg16(uint8_t addr, uint8_t reg) {
    i2c_start();
    i2c_write(addr << 1);
    i2c_assert(I2C_STATUS_SLAW_ACK, "address error");
    i2c_write(reg);
    i2c_assert(I2C_STATUS_DATA_ACK, "device-id error");

    i2c_start();
    i2c_write((addr << 1) | 0x01);
    i2c_assert(I2C_STATUS_SLAR_ACK, "address error");
    uint16_t r = i2c_read(true) | (i2c_read(false) << 8);
    i2c_assert(I2C_STATUS_RCVD_DATA_NACK, "data receive error");
    i2c_stop();

    return r;
}

void i2c_write_reg(uint8_t addr, uint8_t reg, uint8_t data) {
    i2c_start();
    i2c_write(addr << 1);
    i2c_assert(I2C_STATUS_SLAW_ACK, "address error");
    i2c_write(reg);
    i2c_assert(I2C_STATUS_DATA_ACK, "register error");
    i2c_write(data);
    i2c_assert(I2C_STATUS_DATA_ACK, "value error");
    i2c_stop();
}
