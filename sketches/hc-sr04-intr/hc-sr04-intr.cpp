/**
 * This sketch combines the ultra-sonic sensor code with what I learned
 * from working with interrupts. The measurement is done concurrently
 * in an interrupt handler and the main loop just takes the measured
 * distance from a global variable.
 *
 * The use of pulseIn() is not possible as it blocks the whole operation.
 * If used in an interrupt handler, it will simply stop the whole
 * program from doing anything else. I introduced a state machine which
 * handles the steps necessary to send the signal out, set the receive
 * mode and measure the signals runtime.
 *
 * The selection of the timer took a while, simply using a non-prescaled
 * 1µs interval basically kills the processor :-) I am now using an
 * interval around 500Hz which seems to work fine with the main loop
 * still getting enough time to operate without having to disable and
 * enable interrupts.
 *
 * @author Matthias L. Jugel
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
#define TRIG 9
#define ECHO 10

void setup() {
    Serial.begin(115200);
    Serial.println("== INIT ==");
    Serial.println("Be aware that the output of the timing is a concurrent variable!");
    Serial.println("It may have changed between the last distance measurement and output.");

    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);
    pinMode(LED, OUTPUT);

    cli();

    // reset the timer registers
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;

    // 16MHz/8*0.004 - something not 16MHz but fast enough to be able to measure
    OCR1A = 16000000UL / 8 / 1000 / 4 - 1;
    TCCR1B |= _BV(CS11); // prescale 8 selected
    TCCR1B |= _BV(WGM12); // CTC mode
    TIMSK1 |= _BV(OCIE1A); // timer compare interrupt

    sei();
}

// the steps for our little state machine
#define TRIG_INIT 0
#define TRIG_SEND 1
#define TRIG_STOP 2
#define ECHO_START 3
#define ECHO_READ 4

#define MAX_TIMING_LOOPS 8000

// the sensor state and a counter which makes the interrupt skip
volatile int sensorState = TRIG_INIT;

// the final calculated distance
volatile unsigned long distance = 0;
// our timing (we remember when we started and how many loops it took)
// the loops are needed to detect error situations
volatile unsigned long timing = 0;
volatile unsigned long timingStart = 0;
volatile int timingLoops = 0;

/**
 * In the interrupt routing we manage the trigger and measurement using the ultra-sonic sensor.
 */
ISR(TIMER1_COMPA_vect) {
    switch (sensorState) {
        case TRIG_INIT:
            // initiate the trigger to send out sound
            digitalWrite(TRIG, LOW);
            sensorState = TRIG_SEND;
            break;

        case TRIG_SEND:
            digitalWrite(TRIG, HIGH);
            sensorState = TRIG_STOP;
            break;

        case TRIG_STOP:
            digitalWrite(TRIG, LOW);
            sensorState = ECHO_START;
            timingLoops = 0;
            break;

        case ECHO_START:
            if (digitalRead(ECHO) == HIGH) {
                timingStart = micros();
                sensorState = ECHO_READ;
            }
            if (++timingLoops > MAX_TIMING_LOOPS) {
                sensorState = TRIG_INIT;
            }
            break;

        case ECHO_READ:
            if (digitalRead(ECHO) == LOW) {
                timing = micros() - timingStart;
                distance = (timing / 2) / 29 / 2;
                sensorState = TRIG_INIT;
            }
            if (++timingLoops > MAX_TIMING_LOOPS) {
                sensorState = TRIG_INIT;
            }
            break;

        default:
            Serial.println("ILLEGAL STATE");
            sensorState = TRIG_INIT;
            break;
    }
}

int toggleState = HIGH;

/**
 * Our normal main loop. It is stopped whenever the interrupt occurs.
 */
void loop() {
    if (distance >= 300 || distance <= 0) {
        Serial.print("out of range");
    } else {
        Serial.print("range: ");
        Serial.print(distance);
        Serial.print("cm");
    }
    Serial.print(" / timing: ");
    Serial.print(timing);
    Serial.println("µs");
    delay(1000);

}
