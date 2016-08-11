/**
 * i2c scanner (goes through list of available addresses)
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

#include <Arduino.h>
#include <Wire.h>

#define I2C_ADDR_START 0b00000000
#define I2C_ADDR_END 0b0111111

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#pragma clang diagnostic ignored "-Wreturn-stack-address"

void setup() {
  Serial.begin(BAUD);
  pinMode(6, INPUT);

  // blink a few times before we start
  pinMode(13, OUTPUT);
  uint8_t on = 1;
  for (uint8_t i = 3; i > 0; i--) {
    printf(".");
    digitalWrite(13, on);
    on = ~on;
    _delay_ms(3000);
  }
}

void loop(void) {
  Serial.println("\n== Two Wire Interface Test");

  Serial.println("Scanning for TWI (i2c) devices...");
  while (1) {
    bool found = false;
    for (uint8_t address = I2C_ADDR_START; address < I2C_ADDR_END; address++) {
      Serial.print("\r        \r0x");
      Serial.print(address);
      Serial.print("<");
      Wire.begin();
      Wire.beginTransmission(address);
      uint8_t status = Wire.endTransmission();
      Serial.print("0x");
      Serial.print(status, 16);
      if (!status) {
        found = true;
        Serial.print("\rFOUND DEVICE: ");
        Serial.print(address, 16);
      }
      Wire.end();
      Serial.print(">");
      _delay_ms(10);
    }
    if (!found) Serial.println("\rNOTHING FOUND!");
    else Serial.println();
  }
  puts("\nScanning done. Reset manually.");
//    // do some low level blinking until the watchdog hits
//    while (1) {
//        PORTB ^= _BV(PORTB5);
//        _delay_ms(250);
//    };
}

#pragma clang diagnostic pop
