/* 
 *  An interrupt example program.
 *  
 *  It sets up an interrupt at X kHz which is used to call the interrupt routing. The interrupt
 *  routing then executes a piece of code while everything else is on hold.
 *  
 *  The max value for the overflow counter (OCR1A) must be less than or equal to 0xFFFF (65535)
 *  The PRESCAL_XX defines account for exactly 1s at the selected prescale (the number by which
 *  the 16MHz signal is divided into).
 *  
 *  Thus if we want to interrupt less than 1s we should use one NONE - 64 or if we want to set 
 *  the interval at longer than 1s use 256 or 1024. However, in case we want to run a routine
 *  only every 5s or longer we need to keep our own counter.
 *  
 *  // PRESCALE values > 0xFFFF
 *  NONE => 1s == ~244 calls
 *  8    => 1s ==  ~30 calls
 *  64   => 1s ==   ~3 calls
 *  
 *  // PRESCALE values < 0xFFFF
 *  256  => 1s ==    1 call (no multiplier possible)
 *  1024 => 1s ==    1 call (up to multiplier 4)
 *  
 *  These numbers help you to calculate your actual interval (most useful if it is below 1s):
 *  Either adjust the number of times the method is called, or in a more practical way, adjust 
 *  the counter value (which is 0xFFFF for NONE - 64) to a lower value that matches your targeted
 *  time interval:
 *  
 *  LOOPS = (FREQUENCY / INTERVAL) / 0xFFFF
 *  
 *  FREQUENCY is 0xF42400 (NONE, 16MHz), 0x1E8480 (8), 0x3D090 (64)
 *  INTERVAL is in seconds and may be 1000 * 125 for 125ms
 *  
 *  For a 1µs interval we need no extra loops and can set the OCR1A to 0x10 directly. Just multiply
 *  that number for microsecond intervals.
 * 
 *  more info: 
 *  https://sites.google.com/site/qeewiki/books/avr-guide/timers-on-the-atmega328
 *  http://playground.arduino.cc/Main/TimerPWMCheatsheet
 *  
 * == LICENSE ==
 * Copyright 2015 ubirch GmbH
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
  */

#include <Arduino.h>

#define LED 13

#define SECOND_LOOPS_NONE 0xF4
#define SECOND_LOOPS_8 0x1E
#define SECOND_LOOPS_64 0x03

#define PRESCALE_NONE 0xFFFF // 0xF42400
#define PRESCALE_8    0xFFFF // 0x1E8480
#define PRESCALE_64   0xFFFF // 0x3D090
#define PRESCALE_256  0xF424
#define PRESCALE_1024 0x3D09

// the prescale counter contains the calls we need to skip to get close to our target interval
// change this to SECOND_LOOPS_xx if needed
volatile int prescaleCounter = SECOND_LOOPS_NONE;
volatile uint8_t toggleState = HIGH;

void setup() {
    Serial.begin(115200);
    Serial.println("== INIT");

    pinMode(LED, OUTPUT);

    cli();

    // we should not use timer0 as it affects delay() etc. (if using arduino code)

    // reset the timer registers
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;

    // (Hz/Prescaler)*seconds-1
    OCR1A = PRESCALE_NONE - 1; // run every second
    TCCR1B |= _BV(CS10); // must match prescale value
    TCCR1B |= _BV(WGM12); // CTC Mode
    TIMSK1 |= _BV(OCIE1A); // Timer compare interrupt

    sei();
}

/**
 * The interrupt routine executed after a timer interrupt occured.
 * Keep in mind that everything is stopped while this routine runs.
 */
ISR(TIMER1_COMPA_vect) {
    if (--prescaleCounter > 0) return;
    prescaleCounter = SECOND_LOOPS_NONE;

    Serial.println("timer1: interrupt()");
    toggleState = toggleState == HIGH ? LOW : HIGH;
    digitalWrite(LED, toggleState);
}

/**
 * Our normal main loop. It is stopped whenever the interrupt occurs.
 */
void loop() {
    // we can do something here ...
    Serial.println("looping away ...");
    delay(1000);
}
