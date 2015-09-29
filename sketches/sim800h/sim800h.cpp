/** 
 * A simple modem terminal.
 * 
 * This sketch simply proxies the serial port input and output
 * to the SIM800H chip on board of the ubirch #1. Makes it easy
 * to explore the AT command set.
 * 
 * Use a real serial terminal to access the ubirch #1:
 * 
 * $ screen /dev/cu.SLAB_USBtoUART 115200
 * 
 * Exchange the device with the one the Arduino IDE uses.
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
#include <SoftwareSerial.h>

#ifndef BAUD
#   define BAUD 115200
#endif

#define WATCHDOG 6
#define SIM800H_RX 2
#define SIM800H_TX 3
#define SIM800H_KEY 7
#define SIM800H_PS 8

SoftwareSerial sim800h = SoftwareSerial(SIM800H_TX, SIM800H_RX);

void setup() {
    // disable the external watchdog
    pinMode(WATCHDOG, OUTPUT);

    // setup baud rates for serial and modem
    Serial.begin(BAUD);
    sim800h.begin(19200);

    cli();

    // reset the timer registers
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;

    OCR1A = (uint16_t) (16000000UL / 8 - 1); // every 128 microseconds
    TCCR1B |= _BV(CS01); // prescale 8 selected (still fast enough)
    TCCR1B |= _BV(WGM12); // CTC mode
    TIMSK1 |= _BV(OCIE1A); // timer compare interrupt

    sei();

    // query generic information and registration status
    delay(3000);
    Serial.println();
    // ATI - display information
    // AT+CREG? - display information on network registration
    // AT+CMGF=1 - SMS text mode
    // AT+CMGL="ALL" - list all available SMS

//    // check if the chip is already awake, otherwise start wakeup
//    pinMode(SIM800H_KEY, OUTPUT);
//    do {
//        digitalWrite(SIM800H_KEY, HIGH);
//        delay(10);
//        digitalWrite(SIM800H_KEY, LOW);
//        delay(1100);
//        digitalWrite(SIM800H_KEY, HIGH);
//        delay(2000);
//        Serial.print(digitalRead(SIM800H_PS) ? '!' : '.');
//    } while (digitalRead(SIM800H_PS) == LOW);
//    // make pin unused (do not leak)
//    pinMode(SIM800H_KEY, INPUT_PULLUP);
//    Serial.println();

    sim800h.println(F("ATE1"));
    delay(100);
    sim800h.println(F("AT+SAPBR=\"APN\",\""
                              FONA_APN
                              "\""));
    delay(1000);
    sim800h.println(F("AT+SAPBR=\"USER\",\""
                              FONA_USER
                              "\""));
    delay(1000);
    sim800h.println(F("AT+SAPBR=\"PWD\",\""
                              FONA_PASS
                              "\""));
    delay(1000);
    sim800h.println(F("AT+CGATT=1"));


//    sim800h.println(F("AT+SMTPSRV=mail.jugel.info,25"));
//    sim800h.println(F("AT+SMTPFROM=leo@ubirch.com,leo"));
//    sim800h.println(F("AT+SMTPRCPT=0,0,trigger@recipe.ifttt.com"));
//    sim800h.println(F("AT+SMTPSUB=ubirch no1 here"));
//    sim800h.println(F("AT+SMTPBODY=12"));
//    sim800h.println(F("Hello Alice!"));
//    sim800h.println(F("AT+SMTPSEND"));
}

// read what is available from the serial port and send to modem
ISR(TIMER1_COMPA_vect) {
    while (Serial.available() > 0) sim800h.write((uint8_t) Serial.read());
}

// the main loop just reads the responses from the modem and
// writes them to the serial port
void loop() {
    while (sim800h.available() > 0) Serial.write(sim800h.read());
}
