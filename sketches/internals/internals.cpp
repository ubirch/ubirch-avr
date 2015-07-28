/* 
 * A sketch to read internal voltage and temperature.
 * Based on  https://code.google.com/p/tinkerit/wiki
 *
 * Found that sprintf() doesn't include floating point
 * by default.
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

#define WATCHDOG 6
#define LED 13

void setup() {
    pinMode(WATCHDOG, OUTPUT);
    pinMode(LED, OUTPUT);

    Serial.begin(115200);

    cli();

    // reset the timer registers
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;

    OCR1A = 16000000UL / 1024 - 1;
    TCCR1B |= _BV(CS10); // prescale 8 selected
    TCCR1B |= _BV(WGM12); // CTC mode
    TIMSK1 |= _BV(OCIE1A); // timer compare interrupt

    sei();

    delay(3000);
}

volatile float a = 0.5;

// do some work :-)
ISR(TIMER1_COMPA_vect) {
    a = cos(a);
}

// https://code.google.com/p/tinkerit/wiki/SecretThermometer
long readTemp() {
    long result;
    // Read temperature sensor against 1.1V reference
    ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(MUX3);
    delay(2); // Wait for Vref to settle
    ADCSRA |= _BV(ADSC); // Convert
    while (bit_is_set(ADCSRA, ADSC));
    result = ADCL;
    result |= ADCH << 8;
    result = (result - 125) * 1075;
    return result;
}

// https://code.google.com/p/tinkerit/wiki/SecretVoltmeter
long readVcc() {
    long result;
    // Read 1.1V reference against AVcc
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
    delay(2); // Wait for Vref to settle
    ADCSRA |= _BV(ADSC); // Convert
    while (bit_is_set(ADCSRA, ADSC));
    result = ADCL;
    result |= ADCH << 8;
    result = 1126400L / result; // Back-calculate AVcc in mV
    return result;
}

void blink() {
    uint8_t toggleState = LOW;
    for (byte i = 0; i < 10; i++) {
        toggleState = toggleState == HIGH ? LOW : HIGH;
        digitalWrite(LED, toggleState);
        delay(100);
    }
}

/**
 * Our normal main loop. It is stopped whenever the interrupt occurs.
 */
void loop() {
    double temp = readTemp() / 10000.0;
    int t = (int) temp;
    int t1 = (int) (temp - (int) temp) * 1000;
    double volt = readVcc() / 1000.0;
    int v = (int) volt;
    int v1 = (int) (volt - (int) volt) * 1000;

    char str[128];
    sprintf(str, "voltage: %d.%dV, temp: %d.%dºC", v, v1, t, t1);
    Serial.println(str);

    delay(30000);
    blink();
}