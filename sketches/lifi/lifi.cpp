#include <avr/sleep.h>
#include <avr/wdt.h>
#include <Adafruit_NeoPixel.h>

#ifndef BAUD
#   define BAUD 9600
#endif

#define PIXEL_PIN 10
#define PIXEL_COUNT 1

#define STATUS 13
#define WATCHDOG 6

#define PAUSE_BLACK (1)
#define BRIGHTNESS (64)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB | NEO_KHZ800);

char string[33] = "01234567890123456789012345678999";
uint32_t states[255];


void setup() {
  Serial.begin(BAUD);

  pinMode(STATUS, OUTPUT);
  pinMode(WATCHDOG, INPUT);

  // initialize NeoPixel
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  wdt_disable();
}

void show_byte(unsigned int d, uint8_t n) {
  for (uint8_t s = 0; s < 3; s++) {
    strip.setPixelColor(0,
                        BRIGHTNESS * (((n >> s * 3) & 0b100) >> 2),
                        BRIGHTNESS * (((n >> s * 3) & 0b010) >> 1),
                        BRIGHTNESS * (((n >> s * 3) & 0b001)));
    strip.show();
    delay(d);
#if defined(PAUSE_BLACK) && PAUSE_BLACK
    strip.setPixelColor(0, 0);
    strip.show();
    delay(d);
#endif
  }
}

void loop() {
  digitalWrite(STATUS, HIGH);

  delay(3000);

  Serial.println("START SERIES");
  for (unsigned int d = 0; d < 50; d += 10) {
    Serial.println(d);
    for (uint8_t n = 0; n < 255; n++) {
      show_byte(d, n);
    }
    strip.setPixelColor(0, 0);
    strip.show();
    delay(1000);
  }
  delay(1000);

  Serial.println("START DATA");
  for (unsigned int d = 0; d < 50; d += 10) {
    Serial.println(d);
    for (uint8_t i = 0; i < 31; i++) {
      show_byte(d, (uint8_t) string[i]);
    }
    strip.setPixelColor(0, 0);
    strip.show();
    delay(1000);
  }

  Serial.println("STOP");

  digitalWrite(STATUS, LOW);
  while (1);

}

