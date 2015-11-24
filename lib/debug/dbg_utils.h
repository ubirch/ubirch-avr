/**
 * debug helpers
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


#ifndef UBIRCH_DBG_UTILS_H
#define UBIRCH_DBG_UTILS_H

#include <stddef.h>

#ifndef NDEBUG
#  include <stdio.h>
#  define DBG_MSG(...) printf(__VA_ARGS__)
#else
#  define DBG_MSG(...)
#endif

// create a string with the bits contained in this byte (not thread safe!)
void print_bits(size_t n, void const *const ptr);

// blink n times and wait 3s in-between
void blink(uint8_t n);

#endif //UBIRCH_DBG_UTILS_H
