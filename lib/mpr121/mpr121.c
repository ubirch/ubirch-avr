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
#include <i2c.h>

#ifndef NDEBUG
#   ifdef i2c_assert
#      undef i2c_assert
#   endif
#   define i2c_assert(e, m)
#endif

void mpr_set(uint8_t reg, uint8_t data) {
    uint8_t ecr = mpr_get(MPR_R_ECR);
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_ECR, ecr & 0xC0); // stop
    i2c_write_reg(MPR_DEVICE_ADDRESS, reg, data);
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_ECR, ecr); // back to configured run mode
}

inline uint8_t mpr_get(uint8_t reg) {
    return i2c_read_reg(MPR_DEVICE_ADDRESS, reg);
}

inline uint16_t mpr_status(void) {
    return i2c_read_reg16(MPR_DEVICE_ADDRESS, MPR_R_TOUCHSTATUS) & 0x0FFF;
}

uint16_t mpr_status_filtered(uint8_t pin) {
    return pin > 12 ? 0 : i2c_read_reg16(MPR_DEVICE_ADDRESS, MPR_R_TOUCHSTATUS_F + 2 * pin);
}

uint16_t mpr_baseline(uint8_t pin) {
    uint16_t baseline = pin > 12 ? 0 : i2c_read_reg(MPR_DEVICE_ADDRESS, MPR_R_BASELINE + pin);
    // Although internally the baseline value is 10-bit, users can only access the
    // 8 MSB of the 10-bit baseline value through the baseline value registers. The
    // read out from the baseline register must be left shift two bits before
    // comparing it with the 10-bit electrode data.
    return baseline << 2;
}

inline void mpr_setup(uint16_t bitmask) {
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_ECR, bitmask);
}

void mpr_set_threshold(uint8_t pin, uint8_t touch, uint8_t release) {
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_TCH_THRESHOLD + 2 * pin, touch);
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_REL_THRESHOLD + 2 * pin, release);
}

void mpr_set_debounce(uint8_t touch, uint8_t release) {
    mpr_set(MPR_R_DEBOUNCE, (release << 4) | touch);
}

uint8_t mpr_reset(void) {
    mpr_set(MPR_R_RESET, 0x63);

    uint8_t cdc_config = i2c_read_reg(MPR_DEVICE_ADDRESS, MPR_R_CDC_CONFIG);
    uint8_t cdt_config = i2c_read_reg(MPR_DEVICE_ADDRESS, MPR_R_CDT_CONFIG);
    if (!(cdc_config == 0x10 && cdt_config == 0x24)) return 0;


    // stop for reconfiguration
    uint8_t ecr = mpr_get(MPR_R_ECR);
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_ECR, ecr & 0xC0);

    // sensors
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_MHD_RISING, 0x01);
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_NHD_RISING, 0x01);
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_NCL_RISING, 0x10);
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_FDL_RISING, 0x20);
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_MHD_FALLING, 0x01);
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_NHD_FALLING, 0x01);
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_NCL_FALLING, 0x10);
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_FDL_FALLING, 0x20);
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_NHD_TOUCHED, 0x01);
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_NCL_TOUCHED, 0x10);
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_FDL_TOUCHED, 0xFF);

    // proximity
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_MHD_PROX_R, 0x0F);
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_NHD_PROX_R, 0x0F);
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_NCL_PROX_R, 0x00);
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_FDL_PROX_R, 0x00);
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_MHD_PROX_F, 0x01);
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_NHD_PROX_F, 0x01);
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_NCL_PROX_F, 0xFF);
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_FDL_PROX_F, 0xFF);
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_NHD_PROX_T, 0x00);
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_NCL_PROX_T, 0x00);
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_FDL_PROX_T, 0x00);

    // autoconfig
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_AUTOCONFIG, 0x00);
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_AUTOCONFIG + 1, 0x00);
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_USL, 0x00);
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_LSL, 0x00);
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_TARGET_LEVEL, 0x00);

    // filter and global CDC/CDT configuration
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_CDC_CONFIG, 0xFF);
    i2c_write_reg(MPR_DEVICE_ADDRESS, MPR_R_CDT_CONFIG, 0x30);

    uint8_t pin = 11;
    while (pin--) mpr_set_threshold(pin, 40, 20);

    mpr_set_debounce(1, 1);
    mpr_setup(MPR_ECR_PROX_12 | MPR_ECR_CL_B_10 | 12); // 5 bits of 10 used for baseline, all 12 electrodes on

    return 1;
}
