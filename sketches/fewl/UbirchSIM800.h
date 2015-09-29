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

#ifndef UBIRCH_FONA_H
#define UBIRCH_FONA_H

#include <Adafruit_FONA.h>

class UbirchSIM800 : public Adafruit_FONA {

public:
    UbirchSIM800(uint8_t rst, uint8_t key, uint8_t ps);

    void shutdown();

    void wakeup();

protected:
    uint8_t _keypin;
    uint8_t _pspin;
    uint16_t _speed;

    SoftwareSerial *_serial;

private:
};

#endif //UBIRCH_FONA_H
