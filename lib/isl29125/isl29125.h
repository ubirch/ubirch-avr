/**
 * Definitions for the ISL 29125 RGB Sensor.
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

#ifndef UBIRCH_ISL29125_H
#define UBIRCH_ISL29125_H

#include <stdint.h>

#define ISL_DEVICE_ADDRESS  0x44
#define ISL_R_DEVICE_ID     0x00
#define ISL_DEVICE_ID       0x7D

// config registers
#define ISL_R_COLOR_MODE    0x01
#define ISL_R_FILTERING     0x02
#define ISL_R_INTERRUPT     0x03

// threshold registers
#define ISL_R_THRESHOLD_LL  0x04
#define ISL_R_THRESHOLD_LH  0x05
#define ISL_R_THRESHOLD_HL  0x06
#define ISL_R_THRESHOLD_HH  0x07

// general status register
#define ISL_R_STATUS        0x08

// the color registers
#define ISL_R_GREEN_L       0x09
#define ISL_R_GREEN_H       0x0A
#define ISL_R_RED_L         0x0B
#define ISL_R_RED_H         0x0C
#define ISL_R_BLUE_L        0x0D
#define ISL_R_BLUE_H        0x0E

// reset command
#define ISL_R_RESET         0x46

#define ISL_SAMPLE_GAMMA_0  1.0
#define ISL_SAMPLE_GAMMA_1  2.2


// ISL_R_MODE values (B0-2 color mode selection)
#define ISL_MODE_POWERDOWN  0b000000
#define ISL_MODE_GREEN      0b000001
#define ISL_MODE_RED        0b000010
#define ISL_MODE_BLUE       0b000011
#define ISL_MODE_STANDBY    0b000100
#define ISL_MODE_RGB        0b000101
#define ISL_MODE_RG         0b000110
#define ISL_MODE_GB         0b000111

// the lux selection also indicates sensible bit width (more bits offer more lux)
#define ISL_MODE_375LUX     0b000000 // B3 0 default is 375 lux
#define ISL_MODE_10KLUX     0b001000 // B3 1
#define ISL_MODE_16BIT      0b000000 // B4 0 16 bit operation
#define ISL_MODE_12BIT      0b010000 // B4 1 12 bit operation
#define ISL_MODE_INT_TRIG   0b100000 // set INT pin to trigger sampling

// ISL_R_FILTERING values (the actual filter values can be beweel 0x00 (NONE) and 0x3F (MAX)
#define ISL_FILTER_IR_RANGE 0x80 // default is not capping the values (top & bottom)
#define ISL_FILTER_IR_NONE  0x00 // no IR filtering (or low)
#define ISL_FILTER_IR_MAX   0x3F // maximum IR filter

// ISL_R_INTERRPT values (how to trigger interrupts, if selected)
#define ISL_INTERRUPT_NONE  0b000000 // do not trigger interrupts
#define ISL_INTERRUPT_GREEN 0b000001 // trigger interrupt on green
#define ISL_INTERRUPT_RED   0b000010 // trigger interrupt on red
#define ISL_INTERRUPT_BLUE  0b000011 // trigger interrupt on blue

// IS_R_INTERRUPT values (interrupt persist control)
#define ISL_INT_PERSIST1    0b000000 // 1 integration cycle
#define ISL_INT_PERSIST2    0b000100 // 2 integration cycles
#define ISL_INT_PERSIST4    0b001000 // 4 integration cycles
#define ISL_INT_PERSIST8    0b001100 // 8 integration cycles

// the default mode triggers interrupts based on thresholds
#define ISL_INT_ON_THRSLD   0b000000 // threshold triggering
#define ISL_INT_ON_SAMPLE   0b010000 // trigger interrupt on a finished sample cycle

// status flags
#define ISL_STATUS_INT      0b000001 // interrupt was triggered
#define ISL_STATUS_ADC_DONE 0b000010 // conversion done
#define ISL_STATUS_BROWNOUT 0b000100 // power down or brownout occured
#define ISL_STATUS_GREEN    0b010000 // green being converted
#define ISL_STATUS_RED      0b100000 // red being converted
#define ISL_STATUS_BLUE     0b110000 // blue being converted

// 48 bit color value (the maximum available), also used for 12 bit color reads
typedef struct RGB48 {
    uint16_t red;
    uint16_t green;
    uint16_t blue;
} rgb48;

// 24 bit color value (downsampled)
typedef struct RGB24 {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} rgb24;

/**
 * Set up a value in a register on the sensor.
 * @param reg the register to write
 * @param data the value to write
 */
void isl_set(uint8_t reg, uint8_t data);

/**
 * Get current setup from a register on the sensor.
 * @param reg the register to write
 */
uint8_t isl_get(uint8_t reg);

/**
 * Reset the sensor.
 * A non-zero return indicates an error condition.
 */
uint8_t isl_reset(void);

uint16_t isl_read_red(void);

uint16_t isl_read_green(void);

uint16_t isl_read_blue(void);

/**
 * Read full 48 bit color from sensor
 */
rgb48 isl_read_rgb(void);

uint8_t isl_read_red8(void);

uint8_t isl_read_green8(void);

uint8_t isl_read_blue8(void);

/**
 * Read sensor data as 24 bit RGB
 * @param gamma the gamma adjustment >= 1
 */
rgb24 isl_read_rgb24(void);



#endif //UBIRCH_ISL29125_H
