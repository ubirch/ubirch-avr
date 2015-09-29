/**
 * ...
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

#ifndef UBIRCH_I2C_REGISTERS_H
#define UBIRCH_I2C_REGISTERS_H

uint8_t i2c_read_reg(uint8_t addr, uint8_t reg);

uint16_t i2c_read_reg16(uint8_t addr, uint8_t reg);

void i2c_write_reg(uint8_t addr, uint8_t reg, uint8_t data);

#endif //UBIRCH_I2C_REGISTERS_H
