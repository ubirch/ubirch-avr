/**
 * i2c implementation (header)
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

#ifndef UBIRCH_I2C_H
#define UBIRCH_I2C_H

#include <stdint.h>
#include <stdbool.h>
#include <util/twi.h>


#define I2C_STATUS_NO_ERRORS            0x00
#define I2C_STATUS_START_TRANSMITTED    TW_START
#define I2C_STATUS_START_REPEATED       TW_REP_START
#define I2C_STATUS_SLAW_ACK             TW_MT_SLA_ACK
#define I2C_STATUS_SLAW_NACK            TW_MT_SLA_NACK
#define I2C_STATUS_DATA_ACK             TW_MT_DATA_ACK
#define I2C_STATUS_DATA_NACK            TW_MT_DATA_NACK
#define I2C_STATUS_ARB_LOST_OR_NACK     0x38
#define I2C_STATUS_SLAR_ACK             0x40
#define I2C_STATUS_SLAR_NACK            0x48
#define I2C_STATUS_RCVD_DATA_ACK        0x50
#define I2C_STATUS RCVD_DATA_NACK       0x58
#define I2C_STATUS_ARB_LOST             0xE0
#define I2C_STATUS_ARB_LOST_IN_START    0xE1
#define I2C_STATUS_ARB_LOST_IN_STOP     0xE2
#define I2C_STATUS_ARB_LOST_IN_RACK     0xE3
#define I2C_STATUS_ARB_LOST_IN_RNACK    0xE4
#define I2C_STATUS_ARB_LOST_IN WRITE    0xE5
#define I2C_STATUS_UNKNOWN_ERROR        0xF8
#define I2C_STATUS_ILLEGAL_START_STOP   0xFF

// basic two wire functionality
void i2c_init(void);

uint8_t i2c_status(void);

void i2c_start(void);

void i2c_stop(void);

void i2c_write(uint8_t b);

uint8_t i2c_read(bool ack);

#endif //UBIRCH_I2C_H
