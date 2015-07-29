/**
 * A small test program which utilising a an HC-SR04 ultra-sonic sensor board
 * and the built-in led on the ubirch board.
 * 
 * DIGIAL groove to HC-SR04 wiring: 
 *   pin 1 (uno PIN 9) - TRIG
 *   pin 2 (uno PIN 10) - ECHO
 *   pin 3 (+5V) - Vcc
 *   pin 4 (Gnd) - Gnd
 *
 * Ideas taken from 
 * http://www.instructables.com/id/Simple-Arduino-and-HC-SR04-Example
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

#define TRIG 9
#define ECHO 10
#define LED 13


void setup() {
    // to be able to see the messages
    Serial.begin(115200);

    // initialize LED, TRIG and ECHO pins
    pinMode(LED, OUTPUT);
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);
}

/*
 * Small blink function that uses the distance for frequency.
 */
void blink(unsigned long distance) {
    for (byte i = 0; i < 5; i++) {
        digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(min(distance, 200));              // wait for a second
        digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
    }
}

// the loop function runs over and over again forever
void loop() {
    // duration of the measured sound
    long duration;
    // the calculated distance (in cm)
    double distance;

    // initiate the trigger to send out sound
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
    // measure response
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);
    // read response
    duration = pulseIn(ECHO, HIGH);
    distance = (duration / 2) / 29 / 2;

    // give some feedback using the built-in LED
    blink((unsigned long) distance);

    // catch some out of range values or print the distance
    if (distance >= 200 || distance <= 0) {
        Serial.println("Out of range");
    } else {
        Serial.print(distance);
        Serial.println(" cm");
    }
    delay(500);
}

