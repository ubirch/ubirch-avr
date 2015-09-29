/**
 * Functionality for the SSD1306 chip driver for OLED displays.
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

#ifndef UBIRCH_SSD1306_H
#define UBIRCH_SSD1306_H

// the default controller device address
#define OLED_DEVICE_ADDRESS     0x3d

// =================================
#define OLED_ADDRESSING_MODE    0x20

#define OLED_ADDR_MODE_PAGE     0b10
#define OLED_ADDR_MODE_HORIZ    0b00
#define OLED_ADDR_MODE_VERT     0b01

#define OLED_SCROLL_RIGHT       0x26
#define OLED_SCROLL_LEFT        0x27
#define OLED_SCROLL_DIAG_RIGHT  0x29
#define OLED_SCROLL_DIAG_LEFT   0x2A

#define OLED_SCROLL_STOP        0x2E
#define OLED_SCROLL_START       0x2F

#define OLED_START_LINE         0x40

#define OLED_CONTRAST           0x81
#define OLED_CHARGE_PUMP        0x8D

#define OLED_SEGMENT_REMAP0     0xA0
#define OLED_SEGMENT_REMAP1     0xA1

#define OLED_DISPLAY_RESUME     0xA4
#define OLED_DISPLAY_ALL_ON     0xA5

// display mode
#define OLED_DISPLAY_NORMAL     0xA6
#define OLED_DISPLAY_INVERSE    0xA7
#define OLED_MULTIPLEX_RATIO    0xA8

// display on off in general
#define OLED_DISPLAY_OFF        0xAE
#define OLED_DISPLAY_ON         0xAF

// page start address for page addressing mode
#define OLED_PAGE_ADDR_START    0xB0 // 0xB0-0xB7

// output scan direction
#define OLED_SCAN_NORMAL        0xC0
#define OLED_SCAN_REVERSE       0xC8

// display offset (double byte command)
#define OLED_DISPLAY_OFFSET     0xD3

#define OLED_CLOCK_DIV_FREQ     0xD5 // arg: B0-3 clock divide ratio, B4-7 frequency
#define OLED_PRECHARGE_PERIOD   0xD9
#define OLED_COM_PIN_CONFIG     0xDA

#define OLED_VCOM_DESELECT      0xDB
// =================================


/**
 * Reset the controller.
 */
void oled_reset(void);

/**
 * Send a command to the OLED display controller.
 * @param address the controller i2c address
 * @param data the command byte
 */
void oled_cmd(uint8_t address, uint8_t command);

/**
 * Send data to the OLED display controller.
 * @param address the controller i2c address
 * @param data the data byte
 */
void oled_data(uint8_t address, uint8_t data);

/**
 * Clear the display.
 * @param address the controller i2c address
 */
void oled_clear(uint8_t address);

#endif //UBIRCH_SSD1306_H
