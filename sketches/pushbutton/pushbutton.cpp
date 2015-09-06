#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"
#include <avr/sleep.h>
#include <avr/wdt.h>
#include "FastLED.h"

#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4
#define FONA_KEY 7
#define FONA_PS 8

#define NUM_LEDS 1
#define DATA_PIN 10
#define TASTER_PIN 9

#define led 13
#define trigger 6

CRGB leds[NUM_LEDS];

int GameCount;
int WonLost[11];
long Abstand[11];
long Accuracy;
int Status;
int OldStatus;
long StartTime;
long RefTime;
long StopTime;
long ResultTime;
long TimeSpan;
char results[] = "          ";

int flag2 = 1;
char userid[] = "12"; //which player are whe?

SoftwareSerial myfona = SoftwareSerial(FONA_TX, FONA_RX);
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

void UploadResults();

void TurnOnFona();

void GetConnected();

void flushSerial();

void GetDisconnected();

void TurnOffFona();

void sleepabit(int howlong);

void setup() {

    DDRD |= _BV(PIND6);
    wdt_disable();

    FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
    pinMode(TASTER_PIN, INPUT_PULLUP);
    leds[0] = CRGB::Black;
    FastLED.show();
    Serial.begin(BAUD);

    Serial.println("START!");
    OldStatus = -1;
    Status = 0;
    StartTime = 0;
    StopTime = 0;
    ResultTime = 0;
    Accuracy = 300001;
    GameCount = 1;
    delay(100);
    fona.setGPRSNetworkSettings(F("eseye.com"), F("user"), F("pass"));
    pinMode(FONA_KEY, OUTPUT);
}


void loop() {
    // Turn the LED on, then pause
    if (Status != OldStatus) {
        //   Serial.println(Status);
        OldStatus = Status;
    }


    if (Status == 0) {
        TimeSpan = random(3000000, 8000000);
        leds[0] = CRGB::Black;
        FastLED.show();
        StartTime = micros();
        Status = 1;


    }


    if (Status == 1) {

        leds[0] = CRGB::Blue;
        FastLED.show();
        StopTime = micros();
        if ((StopTime - StartTime) > TimeSpan) {
            RefTime = StopTime - StartTime;
            //    Serial.println(RefTime);
            Status = 2;

        }

    }


    if (Status == 2) {
        leds[0] = CRGB::Black;
        FastLED.show();
        Status = 3;

    }

    if (Status == 3) {
        if (digitalRead(TASTER_PIN) == LOW) {
            leds[0] = CRGB::Blue;
            FastLED.show();

            StartTime = micros();
            Status = 4;
        }

    }

    if (Status == 4) {
        if (digitalRead(TASTER_PIN) == HIGH) {
            StopTime = micros();
            leds[0] = CRGB::Black;
            FastLED.show();
            Status = 5;
        }
    }

    if (Status == 5) {

        //  Serial.println(RefTime);
        //  Serial.println(StopTime-StartTime);
        ResultTime = (RefTime) - (StopTime - StartTime);
        ResultTime = abs(ResultTime);
        //   Serial.println(ResultTime);


        Status = 6;
    }


    if (Status == 6) {


        delay(1000);
        Status = 7;

    }


    if (Status == 7) {
        if (ResultTime < Accuracy) {
            leds[0] = CRGB::Green;
            FastLED.show();
            WonLost[GameCount] = 1;
            Abstand[GameCount] = ResultTime;
            results[GameCount] = '1';

        }
        else {
            leds[0] = CRGB::Red;
            FastLED.show();
            WonLost[GameCount] = 0;
            Abstand[GameCount] = ResultTime;
            results[GameCount - 1] = '0';
        }


        delay(3000);
        Status = 8;

    }
    if (Status == 8) {
        // Serial.println(GameCount);
        // Serial.println(WonLost[GameCount]);
        // Serial.println(Abstand[GameCount]);
        // Serial.println("******");
        leds[0] = CRGB::Black;
        FastLED.show();


        delay(2000);

        if (GameCount < 3) {
            GameCount++;
        }
        else {
            UploadResults();
            GameCount = 1;

        }
        Status = 0;

    }


}


void UploadResults() {
    leds[0] = CRGB::Yellow;
    FastLED.show();

    uint16_t returncode;
    TurnOnFona();
    delay(1000);
    myfona.begin(4800);
    fona.begin(myfona);
    delay(3500);
    GetConnected();
    delay(3000);
    fona.enableGPRS(true);
    delay(3000);

    char url[200];
    uint16_t statuscode;
    int16_t length;
    sprintf(url, "%s%s%s%s", "http://api.ubirch.com/rp15/push?id=", userid, "&erg=", results);
    flushSerial();
    if (!fona.HTTP_GET_start(url, &statuscode, (uint16_t *) &length)) {
        Serial.println(F("Get Failed!"));
    }
    fona.HTTP_GET_end();


    delay(100);
    GetDisconnected();
    delay(1000);
    TurnOffFona();
    leds[0] = CRGB::Black;
    FastLED.show();
}


boolean SendATCommand(char Command[], char Value1, char Value2) {
    unsigned char buffer[64];
    unsigned long TimeOut = 20000;
    int count = 0;
    int complete = 0;
    unsigned long commandClock = millis();
    fona.println(Command);
    while (!complete && commandClock <= millis() + TimeOut) {
        while (!fona.available() && commandClock <= millis() + TimeOut);
        while (fona.available()) {
            buffer[count++] = fona.read();
            if (count == 64) break;
        }
        //Serial.write(buffer, count);
        for (int i = 0; i <= count; i++) {
            if (buffer[i] == Value1 && buffer[i + 1] == Value2) complete = 1;
        }
    }
    if (complete == 1) return 1;
    else return 0;
}


void GetConnected() {
    long elapsedTime;
    uint8_t n = 0;
    long startTime;
    startTime = millis(); //start time of the try
    do {
        n = fona.getNetworkStatus();  // Read the Network / Cellular Status
        Serial.print(F("Network status "));
        Serial.print(n);
        Serial.print(F(": "));
        if (n == 0) Serial.println(F("Not registered"));
        if (n == 1) Serial.println(F("Registered (home)"));
        if (n == 2) Serial.println(F("Not registered (searching)"));
        if (n == 3) Serial.println(F("Denied"));
        if (n == 4) Serial.println(F("Unknown"));
        if (n == 5) Serial.println(F("Registered roaming"));
        elapsedTime = millis() - startTime;
        if (elapsedTime > 80000) {
            delay(1000);
            TurnOffFona();
            pinMode(trigger, OUTPUT);
            digitalWrite(led, LOW);
            sleepabit(1800); //if we don't get on the network we will sleep a bit
            startTime = millis(); //reset start-time
            digitalWrite(led, HIGH);
            pinMode(trigger, INPUT);
            TurnOnFona();
            delay(100);
            myfona.begin(BAUD);
            fona.begin(myfona);
            //break;
        }
        ////wdt_reset();
    }
    while (n != 5);
    //if (n !=5)
    //sleepabit(3600);
    ////wdt_reset();
}

void GetDisconnected() {
    fona.enableGPRS(false);
    Serial.println(F("GPRS Serivces Stopped"));
}

void TurnOnFona() {
    Serial.println("Turning on Fona: ");
    while (digitalRead(FONA_PS) == LOW) {
        digitalWrite(FONA_KEY, LOW);
    }
    digitalWrite(FONA_KEY, HIGH);
}

void TurnOffFona() {
    Serial.println("Turning off Fona ");
    while (digitalRead(FONA_PS) == HIGH) {
        digitalWrite(FONA_KEY, LOW);
        //delay(100);
    }
    digitalWrite(FONA_KEY, HIGH);
}

int get_int_len(int value) {
    int l = 1;
    while (value > 9) {
        l++;
        value /= 10;
    }
    return l;
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
// watchdog interrupt
ISR (WDT_vect) {
    //i++;
    //Serial.println("waking up...");
}  // end of WDT_vect

int freeRam() {
    extern int __heap_start, *__brkval;
    int v;
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void flushSerial() {
    while (Serial.available())
        Serial.read();
}
