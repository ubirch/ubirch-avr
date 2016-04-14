//
// Adapted from Stephan Nollers code: https://github.com/snoller/Ardufona2Thingspeak
//
// copy config_template.h to config.h and set the correct values
//
#include "config.h"

#include <avr/sleep.h>
#include <avr/wdt.h>
#include <Arduino.h>
#include <UbirchSIM800.h>
#include <SparkFunISL29125.h>

#ifndef BAUD
#   define BAUD 9600
#endif

SFE_ISL29125 RGB_sensor;

#define led 13
#define trigger 6

UbirchSIM800 sim800h = UbirchSIM800();

int i = 1; //loop counter

void setup() {
    Serial.begin(BAUD);

    pinMode(led, OUTPUT);
    pinMode(trigger, INPUT);
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led, LOW);
    delay(100);

    // edit APN settings in config.h
    sim800h.setAPN(F(FONA_APN), F(FONA_USER), F(FONA_PASS));
}

void sleepabit(int howlong) {
    int i2 = 0;
    delay(100);
    while (i2 < (howlong / 8)) {
        cli();
        delay(100);
        // disable ADC
        //ADCSRA = 0;
        //prepare interrupts
        WDTCSR |= (1 << WDCE) | (1 << WDE);
        // Set Watchdog settings:
        WDTCSR = (1 << WDIE) | (1 << WDE) | (1 << WDP3) | (0 << WDP2) | (0 << WDP1) | (1 << WDP0);
        sei();
        //wdt_reset();
        set_sleep_mode (SLEEP_MODE_PWR_DOWN);
        sleep_enable();
        // turn off brown-out enable in software
        //MCUCR = bit (BODS) | bit (BODSE);
        //MCUCR = bit (BODS);
        sleep_cpu ();
        // cancel sleep as a precaution
        sleep_disable();
        i2++;
    }
    wdt_disable();
}

void GetDisconnected() {
    sim800h.disableGPRS();
    Serial.println(F("GPRS Serivces Stopped"));
}

void GetConnected() {
    sim800h.wakeup();
    while (!sim800h.registerNetwork(60000)) {
        sim800h.shutdown();
        sim800h.wakeup();
    }
    sim800h.enableGPRS();
}

void SendGPS() {
    unsigned int red1 = 0, green1 = 0, blue1 = 0;

    RGB_sensor.init();
    delay(1000);
    while (!(RGB_sensor.readStatus() & FLAG_CONV_DONE)) Serial.print("?");
    Serial.println();
    for (uint8_t n = 0; n < 5; n++) {
        red1 = RGB_sensor.readRed();
        green1 = RGB_sensor.readGreen();
        blue1 = RGB_sensor.readBlue();
    }
    Serial.println("RGB conversion done.");

    Serial.print(red1);
    Serial.print(F(":"));
    Serial.print(green1);
    Serial.print(F(":"));
    Serial.println(blue1);

    char replybuffer[80];
    unsigned long response_length;
    unsigned int http_status;

    GetConnected();

    // get battery status
    uint16_t bat_status, bat_percent, bat_voltage;
    sim800h.println(F("AT+CBC"));
    if (!sim800h.expect_scan(F("+CBC: %d,%d,%d"), &bat_status, &bat_percent, &bat_voltage)) {
        Serial.println("BAT lookup failed");
        bat_percent = 0;
    }
    sim800h.expect_OK();

    // get gsm location
    uint16_t loc_status;
    char *lat = NULL, *lon = NULL;
    sim800h.println(F("AT+CIPGSMLOC=1,1"));
    if (!sim800h.expect_scan(F("+CIPGSMLOC: %d,%s"), &loc_status, replybuffer, 60000)) {
        Serial.println(F("GPS lookup failed"));
    } else {
        lon = strtok(replybuffer, ",");
        lat = strtok(NULL, ",");
    }
    sim800h.expect_OK();

    if (loc_status == 0 && lat && lon) {
        //send the stuff to TP
        char url[300];
        sprintf(url,
                THINGSPEAK_URL "&field1=%d&field2=%d&field3=%d&field4=%d&field5=%d&lat=%s&long=%s",
                red1 >> 8, green1 >> 8, blue1 >> 8, bat_percent, i, lat, lon);
        Serial.println("URL:");
        Serial.println(url);

        http_status = sim800h.HTTP_get(url, response_length);
        if (http_status != 200) {
            Serial.println(F("HTTP GET failed"));
            Serial.println(http_status);
            Serial.println(response_length);
        }
    } else {
        //send the stuff to TP without GPS
        char url[200];
        sprintf(url,
                THINGSPEAK_URL "&field1=%d&field2=%d&field3=%d&field4=%d&field5=%d&lat=0&long=0",
                red1 >> 8, green1 >> 8, blue1 >> 8, bat_percent, i);
        Serial.println("URL (no GPS):");
        Serial.println(url);
        http_status = sim800h.HTTP_get(url, response_length);
        if (http_status != 200) {
            Serial.println(F("HTTP GET failed"));
            Serial.println(http_status);
            Serial.println(response_length);
        }
    }
    GetDisconnected();
    sim800h.shutdown();
}

void loop() {
    digitalWrite(led, HIGH);
    pinMode(trigger, INPUT);
    SendGPS();
    pinMode(trigger, OUTPUT);
    digitalWrite(led, LOW);
    i++;

    sleepabit(3350);
}

// watchdog interrupt
ISR (WDT_vect) {
    //i++;
}  // end of WDT_vect