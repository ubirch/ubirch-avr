//
// Adapted from Stephan Nollers code: https://github.com/snoller/Ardufona2Thingspeak
//
// copy config_template.h to config.h and set the correct values
//
#include "config.h"

#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"

#include "UbirchSIM800.h"

extern "C" {

#include <isl29125.h>
#include <i2c_core.h>
#include <dbg_utils.h>
}


#include <avr/sleep.h>
#include <avr/wdt.h>

// #include <isl29125.h>


#ifndef BAUD
#   define BAUD 9600
#endif

// SFE_ISL29125 RGB_sensor;

#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4
#define FONA_KEY 7
#define FONA_PS 8
#define led 13
#define trigger 6

SoftwareSerial fonaSerial = SoftwareSerial(FONA_TX, FONA_RX);
UbirchSIM800 fona = UbirchSIM800(FONA_RST, FONA_KEY, FONA_PS);

int i = 1; //loop counter

void setup() {
    Serial.begin(BAUD);
    fonaSerial.begin(9600);
    fona.begin(fonaSerial);

    pinMode(led, OUTPUT);
    pinMode(trigger, INPUT);
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led, LOW);
    delay(100);

    i2c_init(I2C_SPEED_400KHZ);
    if (!isl_reset()) {
        puts("could not initialize ISL29125 RGB sensor");
    }

    // set sampling mode, ir filter and interrupt mode
    isl_set(ISL_R_COLOR_MODE, ISL_MODE_RGB | ISL_MODE_10KLUX | ISL_MODE_16BIT);
    isl_set(ISL_R_FILTERING, ISL_FILTER_IR_MAX);
    isl_set(ISL_R_INTERRUPT, ISL_INT_ON_THRSLD);

    printf("read mode: ");
    uint8_t color_mode = isl_get(ISL_R_COLOR_MODE);
    print_bits(1, &color_mode);
    printf("read filter: ");
    uint8_t ir_filtering = isl_get(ISL_R_FILTERING);
    print_bits(1, &ir_filtering);
    printf("read interrupts: ");
    uint8_t intr = isl_get(ISL_R_INTERRUPT);
    print_bits(1, &intr);

    /*
    if (RGB_sensor.init()) {
        for (int i = 0; i < 2; i++) {
            unsigned int trash1 = RGB_sensor.readRed();
            unsigned int trash2 = RGB_sensor.readGreen();
            unsigned int trash3 = RGB_sensor.readBlue();
            delay(1000);
        }
    }
    */


    // edit APN settings in config.h
    fona.setGPRSNetworkSettings(F(FONA_APN), F(FONA_USER), F(FONA_PASS));
}

void flushSerial() {
    while (Serial.available())
        Serial.read();
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
    fona.enableGPRS(false);
    Serial.println(F("GPRS Serivces Stopped"));
}

void GetConnected() {
    fona.wakeup();

    long elapsedTime;
    uint8_t n = 0;
    long startTime;
    startTime = millis(); //start time of the try
    do {
        n = fona.getNetworkStatus();  // Read the Network / Cellular Status
        Serial.print(F("Network status "));
        Serial.print(n);
        Serial.print(F(": "));
        switch (n) {
            case 0:
                Serial.println(F("Not registered"));
                break;
            case 1:
                Serial.println(F("Registered (home)"));
                break;
            case 2:
                Serial.println(F("Not registered (searching)"));
                break;
            case 3:
                Serial.println(F("Denied"));
                break;
            case 4:
                Serial.println(F("Unknown"));
                break;
            case 5:
                Serial.println(F("Registered roaming"));
                break;
            default:
                Serial.println(F("???"));
                break;
        }
        elapsedTime = millis() - startTime;
        if (elapsedTime > 80000) {
            delay(1000);
            fona.shutdown();
            pinMode(trigger, OUTPUT);
            digitalWrite(led, LOW);
            sleepabit(1800); //if we don't get on the network we will sleep a bit
            startTime = millis(); //reset start-time
            digitalWrite(led, HIGH);
            pinMode(trigger, INPUT);
            fona.wakeup();
            delay(100);
            //break;
        }
        ////wdt_reset();
    }
        //be careful - this is code expects a SIM that registeres as roaming - change this to "1" if your SIM is registering normally
    while (n != 1);
    //if (n !=5)
    //sleepabit(3600);
    ////wdt_reset();
}

void SendGPS() {


    while (!(isl_get(ISL_R_STATUS) & ISL_STATUS_ADC_DONE)) putchar('%');
    Serial.println("RGB conversion done.");

    // printf("48bit: ");
    rgb48 rgb = isl_read_rgb();
    // printf("0x%04x%04x%04x rgb48(%u,%u,%u)\n", rgb.green, rgb.red, rgb.blue, rgb.red, rgb.green, rgb.blue);

    //prepare sensor data

    Serial.println(rgb.red);

    char value_red[3 + 1];
    char value_green[3 + 1];
    char value_blue[3 + 1];
    sprintf(value_red, "%d", rgb.red);
    sprintf(value_green, "%d", rgb.green);
    sprintf(value_blue, "%d", rgb.blue);

    char replybuffer[80];
    uint16_t returncode;
    fona.wakeup();
    GetConnected();
    fona.enableGPRS(true);
    while (fona.GPRSstate() == 0) Serial.print(".");
    Serial.println("! GPRS OK");

    // force reconnect
    if (fona.sendCheckReply(F("AT+SAPBR=3,1,\"APN\",\""
                                      FONA_APN
                                      "\""), F("OK")), -1)
        Serial.println(F("FONA: APN SET"));
    if (fona.sendCheckReply(F("AT+SAPBR=0,1"), F("OK")), -1) Serial.println(F("FONA: DISCONNECTED"));
    if (fona.sendCheckReply(F("AT+SAPBR=1,1"), F("OK")), -1) Serial.println(F("FONA: CONNECTED"));

    while (fona.GPRSstate() == 0) Serial.print(".");
    Serial.println("! GPRS OK");

    delay(3000);

    //get battery status
    uint16_t vbat;
    fona.getBattPercent(&vbat);
    char value_bat[20];
    sprintf(value_bat, "%d", vbat);
    Serial.print("Battery :");
    Serial.println(value_bat);

    //get number of loops
    char loops[10];
    sprintf(loops, "%d", i);

    //try to get GPS info
    if (!fona.getGSMLoc(&returncode, replybuffer, 250))
        Serial.println(F("GPS Lookup Failed!"));
    if (returncode == 0) {
        //parse lat and lon
        char *lat;
        char *lon;
        char delimiter[] = ",";
        char *ptr;
        ptr = strtok(replybuffer, delimiter);
        int h = 0;
        while (ptr != NULL) {
            if (h == 0)
                lat = ptr;
            if (h == 1)
                lon = ptr;
            ptr = strtok(NULL, delimiter);
            h++;
        } //while ptr end

        //send the stuff to TP
        char url[300];
        uint16_t statuscode;
        int16_t length;
        sprintf(url,
                "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s", THINGSPEAK_URL,
                "&field1=", value_red,
                "&field2=", value_green,
                "&field3=", value_blue,
                "&field4=", value_bat,
                "&field5=", loops,
                "&lat=", lat,
                "&long=", lon);
        flushSerial();
        Serial.println("URL:");
        Serial.println(url);
        if (!fona.HTTP_GET_start(url, &statuscode, (uint16_t *) &length)) {
            Serial.println(F("Get Failed!"));
            Serial.println(statuscode);
            Serial.println(length);
        }
        fona.HTTP_GET_end();
    } //returncode 0 end
    else {
        //send the stuff to TP without GPS
        char url[200];
        uint16_t statuscode;
        int16_t length;
        sprintf(url,
                "%s%s%s%s%s%s%s%s%s%s%s", THINGSPEAK_URL,
                "&field1=", value_red,
                "&field2=", value_green,
                "&field3=", value_blue,
                "&field4=", value_bat,
                "&field5=", loops);
        Serial.println("URL (no GPS):");
        Serial.println(url);
        flushSerial();
        if (!fona.HTTP_GET_start(url, &statuscode, (uint16_t *) &length)) {
            Serial.println(F("Get Failed!"));
            Serial.println(statuscode);
            Serial.println(length);
        }
        delay(5000);
        fona.HTTP_GET_end();

    }
    delay(100);
    GetDisconnected();
    delay(1000);
    fona.shutdown();
}

void loop() {
    digitalWrite(led, HIGH);
    pinMode(trigger, INPUT);

    if (i % 3 == 0) {
        SendGPS();
        delay(1000);
    }
    else {
        SendGPS();
        delay(1000);
        //Send2TP();
    }

    delay(100);
    fona.shutdown();
    delay(1000);
    pinMode(trigger, OUTPUT);
    digitalWrite(led, LOW);
    i++;

    sleepabit(600);
}


int get_int_len(int value) {
    int l = 1;
    while (value > 9) {
        l++;
        value /= 10;
    }
    return l;
}

// watchdog interrupt
ISR (WDT_vect) {
    //i++;
    Serial.println("waking up...");
}  // end of WDT_vect

int freeRam() {
    extern int __heap_start, *__brkval;
    int v;
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

