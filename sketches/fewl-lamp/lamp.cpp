//
// Adapted from Stephan Nollers code
//
// copy config_template.h to config.h and set the correct values
//
#include "UbirchSIM800.h"
#include "config.h"

#include <avr/sleep.h>
#include <avr/wdt.h>
#include <Adafruit_NeoPixel.h>

#ifndef BAUD
#   define BAUD 9600
#endif

#define SLEEP_CYCLES 200
//#define SLEEP_CYCLES 1600

#define PIXEL_PIN 10
#define PIXEL_COUNT 1

#define led 13
#define trigger 6

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
UbirchSIM800 sim800h = UbirchSIM800();

int loop_counter = 1; //loop counter
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

    // initialize NeoPixel
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'

    wdt_disable();
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
        sleep_cpu();
        // cancel sleep as a precaution
        sleep_disable();
        i2++;
    }
    wdt_disable();
}

bool getBatteryStatus(uint16_t &bat_status, uint16_t &bat_percent, uint16_t &bat_voltage) {
    sim800h.println(F("AT+CBC"));
    if (!sim800h.expect_scan(F("+CBC: %d,%d,%d"), &bat_status, &bat_percent, &bat_voltage)) {
        Serial.println("BAT lookup failed");
        bat_percent = 0;
    }
    return sim800h.expect_OK();
}

void GetRGB() {
    sim800h.wakeup();
    while (!sim800h.registerNetwork(60000)) {
        sim800h.shutdown();
        sim800h.wakeup();
    }
    sim800h.enableGPRS();

    // get battery status
    uint16_t bat_status = 0, bat_percent = 0, bat_voltage = 0;
    if (!getBatteryStatus(bat_status, bat_percent, bat_voltage)) {
        Serial.println("BAT status failed");
    }

    unsigned long response_length;
    unsigned int http_status;
    char url[400];

    sprintf(url, GETRGB_URL "&loops=%d&bat=%d", loop_counter, bat_percent);
    http_status = sim800h.HTTP_get(url, response_length);

    // if the status is not OK or the response length value is too big (>63) we fail
    if (http_status != 200 || response_length > 63) {
        Serial.println(F("HTTP GET failed"));
        Serial.println(http_status);
        Serial.println(response_length);
        sim800h.disableGPRS();
        sim800h.shutdown();
    } else {
        // take into account the 0 at the end of strings
        char response_buffer[64] = {0};
        size_t received_length = sim800h.HTTP_read(response_buffer, 0, (size_t) response_length);
        sim800h.disableGPRS();
        sim800h.shutdown();

        if (received_length == response_length) {
            const char delimiter[2] = ":";
            uint8_t red, green, blue, flag;
            red = (uint8_t) atoi(strtok(response_buffer, delimiter));
            green = (uint8_t) atoi(strtok(NULL, delimiter));
            blue = (uint8_t) atoi(strtok(NULL, delimiter));
            flag = (uint8_t) atoi(strtok(NULL, delimiter));

            if (flag == 1) {
                Serial.println(F("blink lamp"));

                for (int z = 0; z < 256; z++) {
                    strip.setPixelColor(0, z, 255 - z, 0);
                    strip.show();
                    delay(3);
                }
                for (int z = 0; z < 256; z++) {
                    strip.setPixelColor(0, 0, z, 255 - z);
                    strip.show();
                    delay(3);
                }
                for (int z = 0; z < 7; z++) {
                    strip.setPixelColor(0, 255, 0, 0);
                    strip.show();
                    delay(100);
                    strip.setPixelColor(0, 0, 255, 0);
                    strip.show();
                    delay(100);
                    strip.setPixelColor(0, 0, 0, 255);
                    strip.show();
                    delay(100);
                }
            }
            delay(1000);
            Serial.println(F("set lamp color"));
            Serial.print(red);
            Serial.print(F(":"));
            Serial.print(green);
            Serial.print(F(":"));
            Serial.println(blue);
            strip.setPixelColor(0, red, green, blue);
            strip.show();
            delay(100);
        }
    }
}

void loop() {
    digitalWrite(led, HIGH);
    pinMode(trigger, INPUT);

    GetRGB();

    pinMode(trigger, OUTPUT);
    digitalWrite(led, LOW);
    loop_counter++;

    sleepabit(SLEEP_CYCLES);
}

// the ISR is necessary to allow the CPU from actually sleeping
ISR (WDT_vect) { }

