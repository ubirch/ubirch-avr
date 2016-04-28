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

#include "isl29125.h"
#include <i2c.h>

uint8_t isl_set(uint8_t reg, uint8_t data) {
  return i2c_write_reg(ISL_DEVICE_ADDRESS, reg, data);
}

uint8_t isl_get(uint8_t reg) {
  return i2c_read_reg(ISL_DEVICE_ADDRESS, reg);
}

uint8_t isl_reset(void) {
  i2c_write_reg(ISL_DEVICE_ADDRESS, 0x00, ISL_R_RESET);

  if (i2c_read_reg(ISL_DEVICE_ADDRESS, ISL_R_DEVICE_ID) != ISL_DEVICE_ID) return 0;

  uint8_t check = 0x00;
  check |= i2c_read_reg(ISL_DEVICE_ADDRESS, ISL_R_COLOR_MODE);
  check |= i2c_read_reg(ISL_DEVICE_ADDRESS, ISL_R_FILTERING);
  check |= i2c_read_reg(ISL_DEVICE_ADDRESS, ISL_R_INTERRUPT);
  check |= i2c_read_reg(ISL_DEVICE_ADDRESS, ISL_R_STATUS);
  if (check != 0) return check;

  return 1;
}

uint16_t isl_read_red(void) {
  return i2c_read_reg16(ISL_DEVICE_ADDRESS, ISL_R_RED_L);
}

uint16_t isl_read_green(void) {
  return i2c_read_reg16(ISL_DEVICE_ADDRESS, ISL_R_GREEN_L);
}

uint16_t isl_read_blue(void) {
  return i2c_read_reg16(ISL_DEVICE_ADDRESS, ISL_R_BLUE_L);
}

static inline uint8_t downsample(uint16_t c) {
  return c >> ((i2c_read_reg(ISL_DEVICE_ADDRESS, ISL_R_COLOR_MODE) & ISL_MODE_12BIT) ? 4 : 8);
}

uint8_t isl_read_red8(void) {
  return downsample(isl_read_red());
}

uint8_t isl_read_green8(void) {
  return downsample(isl_read_green());;
}

uint8_t isl_read_blue8(void) {
  return downsample(isl_read_blue());
}

rgb48 isl_read_rgb(void) {
  rgb48 result = {
          .red = isl_read_red(),
          .green = isl_read_green(),
          .blue = isl_read_blue()
  };
  return result;
}

rgb24 isl_read_rgb24(void) {
  rgb24 result = {
          .red = isl_read_red8(),
          .green = isl_read_green8(),
          .blue = isl_read_blue8()
  };
  return result;
}
