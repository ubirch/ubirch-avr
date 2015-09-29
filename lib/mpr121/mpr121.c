/**
 * Capacityive sensor library (MPR121)
 *
 * The library assumes that the i2c bus has been initialized already.
 * It will only communicate by issuing start/stop conditions and transmitting
 * command and data requests.
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

#include "mpr121.h"

// disable asserts in non-debug code
#include <i2c.h>

#ifndef NDEBUG
#   ifdef i2c_assert
#      undef i2c_assert
#   endif
#   define i2c_assert(e, m)
#endif

void mpr_set(uint8_t reg, uint8_t data) {
    i2c_write_reg(MPR_DEVICE_ADDRESS, reg, data);
}

uint8_t mpr_get(uint8_t reg) {
    return i2c_read_reg(MPR_DEVICE_ADDRESS, reg);
}

uint8_t mpr_reset(void) {
    mpr_set(MPR_R_RESET, 0x63);

    uint8_t cdc_config = i2c_read_reg(MPR_DEVICE_ADDRESS, MPR_R_CDC_CONFIG);
    uint8_t cdt_config = i2c_read_reg(MPR_DEVICE_ADDRESS, MPR_R_CDT_CONFIG);
    if (!(cdc_config == 0x10 && cdt_config == 0x24)) return 0;

    mpr_set(MPR_R_MHD_RISING, 0x01);
    mpr_set(MPR_R_NHD_RISING, 0x01);
    mpr_set(MPR_R_NCL_RISING, 0x0E);
    mpr_set(MPR_R_FDL_RISING, 0x00);

    mpr_set(MPR_R_MHD_FALLING, 0x01);
    mpr_set(MPR_R_NHD_FALLING, 0x05);
    mpr_set(MPR_R_NCL_FALLING, 0x01);
    mpr_set(MPR_R_FDL_FALLING, 0x00);

    mpr_set(MPR_R_NHD_TOUCHED, 0x00);
    mpr_set(MPR_R_NCL_TOUCHED, 0x00);
    mpr_set(MPR_R_FDL_TOUCHED, 0x00);

    mpr_set(MPR_R_DEBOUNCE, 0);
    mpr_set(MPR_R_CDC_CONFIG, 0x10); // default, 16uA charge current
    mpr_set(MPR_R_CDT_CONFIG, 0x20); // 0.5uS encoding, 1ms period

    // enable all electrodes
    mpr_set(MPR_R_ECR, 0x8F);  // start with first 5 bits of baseline tracking

    return 1;
}

uint16_t mpr_status(void) {
    return i2c_read_reg16(MPR_DEVICE_ADDRESS, MPR_R_TOUCHSTATUS) & 0x0FFF;
}

void mpr_debounce(uint8_t touch, uint8_t release) {
    mpr_set(MPR_R_DEBOUNCE, (release << 4) | touch);
}

uint16_t mpr_status_filtered(uint8_t pin) {
    return pin > 12 ? 0 : i2c_read_reg16(MPR_DEVICE_ADDRESS, MPR_R_TOUCHSTATUS_F + 2 * pin);
}

uint16_t mpr_baseline(uint8_t pin) {
    uint16_t baseline = pin > 12 ? 0 : i2c_read_reg16(MPR_DEVICE_ADDRESS, MPR_R_BASELINE + pin);
    // Although internally the baseline value is 10-bit, users can only access the
    // 8 MSB of the 10-bit baseline value through the baseline value registers. The
    // read out from the baseline register must be left shift two bits before
    // comparing it with the 10-bit electrode data.
    return baseline << 2;
}

void mpr_threshold(uint8_t pin, uint8_t touch, uint8_t release) {
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_TCH_THRESHOLD + 2 * pin, touch);
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_REL_THRESHOLD + 2 * pin, touch);
}