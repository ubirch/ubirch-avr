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

#include "UbirchSIM800.h"

// show debug output only in non-release mode
#ifndef NDEBUG
#   define DEBUG(s) Serial.print(F("SIM800: ")); Serial.println(s)
#else
#   define DEBUG(s)
#endif

UbirchSIM800::UbirchSIM800(uint8_t rst, uint8_t key, uint8_t ps) : Adafruit_FONA::Adafruit_FONA(rst) {
    _keypin = key;
    _pspin = ps;
}

void UbirchSIM800::wakeup() {
    DEBUG("wakeup()");
    // check if the chip is already awake, otherwise start wakeup
    if (!sendCheckReply(F("AT"), F("OK"), 5000)) {
        DEBUG("wakeup(): using PWRKEY wakeup procedure");
        pinMode(_keypin, OUTPUT);
        do {
            digitalWrite(_keypin, HIGH);
            delay(10);
            digitalWrite(_keypin, LOW);
            delay(1100);
            digitalWrite(_keypin, HIGH);
            delay(2000);
            DEBUG(digitalRead(_pspin) ? '!' : '.');
        } while (digitalRead(_pspin) == LOW);
        // make pin unused (do not leak)
        pinMode(_keypin, INPUT_PULLUP);
        DEBUG("wakeup(): ok");
    } else {
        DEBUG("wakeup(): already awake");
    }

    if (begin(*mySerial)) {
        DEBUG("wakeup(): initialized");
    } else {
        DEBUG("wakeup(): can't initialize");
    }

}

void UbirchSIM800::shutdown() {
    DEBUG("shutdown()");
    // sendCheckReply() doesn't catch the NORMAL POWER DOWN sequence, possibly due to a missing newline?
    getReply(F("AT+CPOWD=1"), (uint16_t) 5000);
    if (strncmp(replybuffer, "NORMAL POWER DOWN", 17) != 0) {
        if (digitalRead(_pspin) == HIGH) {
            DEBUG("shutdown() using PWRKEY, AT+CPOWD=1 failed");
            pinMode(_keypin, OUTPUT);
            do { digitalWrite(_keypin, LOW); } while (digitalRead(_pspin) == HIGH);
            digitalWrite(_keypin, HIGH);
            pinMode(_keypin, INPUT_PULLUP);
        } else {
            DEBUG("shutdown(): already shut down");
        }
    }
    DEBUG("shutdown(): ok");
}
