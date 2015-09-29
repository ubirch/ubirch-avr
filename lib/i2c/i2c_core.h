/**
 * i2c implementation (header)
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

#ifndef UBIRCH_I2C_H
#define UBIRCH_I2C_H

#include <stdint.h>
#include <stdbool.h>
#include <util/twi.h>

// define some sensible speed defaults
#define I2C_SPEED_50KHZ                 ((((F_CPU /  50000UL) / 1) - 16) / 2)
#define I2C_SPEED_100KHZ                ((((F_CPU / 100000UL) / 1) - 16) / 2)
#define I2C_SPEED_400KHZ                ((((F_CPU / 400000UL) / 1) - 16) / 2)

#define I2C_STATUS_NO_ERRORS            0x00
#define I2C_STATUS_START_TRANSMITTED    TW_START            // 0x08
#define I2C_STATUS_START_REPEATED       TW_REP_START        // 0x10
#define I2C_STATUS_SLAW_ACK             TW_MT_SLA_ACK       // 0x18
#define I2C_STATUS_SLAW_NACK            TW_MT_SLA_NACK      // 0x20
#define I2C_STATUS_DATA_ACK             TW_MT_DATA_ACK      // 0x28
#define I2C_STATUS_DATA_NACK            TW_MT_DATA_NACK     // 0x30
#define I2C_STATUS_ARB_LOST_OR_NACK     TW_MT_ARB_LOST      // 0x38
#define I2C_STATUS_SLAR_ACK             TW_MR_SLA_ACK       // 0x40
#define I2C_STATUS_SLAR_NACK            TW_MR_SLA_NACK      // 0x48
#define I2C_STATUS_RCVD_DATA_ACK        TW_MR_DATA_ACK      // 0x50
#define I2C_STATUS_RCVD_DATA_NACK       TW_MR_DATA_NACK     // 0x58
#define I2C_STATUS_ARB_LOST             0xE0
#define I2C_STATUS_ARB_LOST_IN_START    0xE1
#define I2C_STATUS_ARB_LOST_IN_STOP     0xE2
#define I2C_STATUS_ARB_LOST_IN_RACK     0xE3
#define I2C_STATUS_ARB_LOST_IN_RNACK    0xE4
#define I2C_STATUS_ARB_LOST_IN WRITE    0xE5
#define I2C_STATUS_UNKNOWN_ERROR        0xF8
#define I2C_STATUS_ILLEGAL_START_STOP   0xFF

// basic two wire functionality

/**
 * initalize the i2c communication
 */
void i2c_init(uint8_t speed);

/**
 * request the current i2c status
 */
uint8_t i2c_status(void);

/**
 * initiate a START condition
 */
void i2c_start(void);

/**
 * conclude a transmission and free the bus
 */
void i2c_stop(void);

/**
 * cmd a single byte to the bus
 * @param b the byte to send
 */
void i2c_write(uint8_t b);

/**
 * read a single byte from the bus
 * @param ack == true will acknowledge the read, which is necessary if we request more data
 */
uint8_t i2c_read(bool ack);

/**
 * assert that a certain bus condition is true
 * @param the expected result
 * @param message the message to display, or NULL
 */
#ifndef NDEBUG

#  include <stdio.h>

#  define i2c_assert(expected, message) if(TWSR != expected) \
      printf("i2c: status: 0x%02x (expected 0x%02x): %s\n", TWSR, expected, message);
#else
#  define i2c_assert(expected, message)
#endif

#endif //UBIRCH_I2C_H
