/**
 * debug helpers
 *
 * @author Matthias L. Jugel
 *
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

#include "dbg_utils.h"

// create a string with the bits contained in this byte (not thread safe!)
char *bits(uint8_t n) {
    static char buf[8];
    for (uint8_t i = 0; i < 7; i++)
        *(buf + i) = (char) ((n >> i) & 1 ? '1' : '0');
    return buf;
}