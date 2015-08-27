/**
 * Morse Code
 *
 * Encoding taken from the example in http://cs.stackexchange.com/questions/39920/is-morse-code-binary-ternary-or-quinary
 * The encoding works as follow:
 * 
 * Symbols:
 * 
 * Low, off, etc. (0)
 * High, on, etc. (1)
 * Encoding Examples:
 *
 * Signal separator: 0
 * Letter separator: 000
 * Word separator: 0000000
 * Three dots: 10101
 * Three dashes: 11101110111
 * The word "son": 101010001110111011100011101
 * The phrase "is a": 10100010101000000010111
 *
 * Pros:
 * - Minimal symbol set size
 * - Easily managed with typical digital logic
 *
 * - Easy to decode into audible Morse Code (presuming no validation is done); stateless decoding and does not require storing information about the timing of symbols (every symbol is the same length).
 * Cons:
 * - Long encodings.
 * - Many opportunities for invalid encodings.
 * - Individual symbol meanings (0, 1) depend on context.
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

#ifndef UBIRCH_MORSE_H_H
#define UBIRCH_MORSE_H_H

#include <avr/pgmspace.h>

#define M_SIGNAL_SEP 1
#define M_LETTER_SEP 3
#define M_WORD_SEP   7

#define S(v) (char *) pgm_read_word(&(v))

#define M_CODE(letter) ((char *)pgm_read_word(&(M_ALPHABET[(((letter)-'a') < 0 ? ((letter)-'1')+('z'-'a') : (letter)-'a')])))

// a-z
const char m_a[] PROGMEM = ".-";
const char m_b[] PROGMEM = "-...";
const char m_c[] PROGMEM = "-.-.";
const char m_d[] PROGMEM = "-..";
const char m_e[] PROGMEM = ".";
const char m_f[] PROGMEM = "..-.";
const char m_g[] PROGMEM = "--.";
const char m_h[] PROGMEM = "....";
const char m_i[] PROGMEM = "..";
const char m_j[] PROGMEM = ".---";
const char m_k[] PROGMEM = "-.-";
const char m_l[] PROGMEM = ".-..";
const char m_m[] PROGMEM = "--";
const char m_n[] PROGMEM = "-.";
const char m_o[] PROGMEM = "---";
const char m_p[] PROGMEM = ".--.";
const char m_q[] PROGMEM = "--.-";
const char m_r[] PROGMEM = ".-.";
const char m_s[] PROGMEM = "...";
const char m_t[] PROGMEM = "-";
const char m_u[] PROGMEM = "..-";
const char m_v[] PROGMEM = "...-";
const char m_w[] PROGMEM = ".--";
const char m_x[] PROGMEM = "-..-";
const char m_y[] PROGMEM = "-.--";
const char m_z[] PROGMEM = "--..";
// 1-9, 0
const char m_1[] PROGMEM = ".----";
const char m_2[] PROGMEM = "..---";
const char m_3[] PROGMEM = "...--";
const char m_4[] PROGMEM = "....-";
const char m_5[] PROGMEM = ".....";
const char m_6[] PROGMEM = "-....";
const char m_7[] PROGMEM = "--...";
const char m_8[] PROGMEM = "---..";
const char m_9[] PROGMEM = "----.";
const char m_0[] PROGMEM = "-----";

// the morse alphabet represented in max 20 bits per character
const char *const M_ALPHABET[] PROGMEM = {
        m_a, m_b, m_c, m_d, m_e, m_f, m_g, m_h, m_i, m_j, m_k, m_l, m_m,
        m_n, m_o, m_p, m_q, m_r, m_s, m_t, m_u, m_v, m_w, m_x, m_y, m_z,
        m_1, m_2, m_3, m_4, m_5, m_6, m_7, m_8, m_9, m_0
};


#endif //UBIRCH_MORSE_H_H
