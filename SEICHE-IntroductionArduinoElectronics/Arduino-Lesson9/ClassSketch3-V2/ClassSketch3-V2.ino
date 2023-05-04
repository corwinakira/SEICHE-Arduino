#include <FastLED.h>

// How many leds in your strip?
#define NUM_LEDS 30

// For led chips like WS2812, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
// Clock pin only needed for SPI based chipsets when not using hardware SPI
#define DATA_PIN 5
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define BRIGHTNESS 30

#define RAINBOW true

// Define the array of leds
CRGB leds[NUM_LEDS];
uint8_t i;
uint8_t randhue = 0;
int pxidx,  lastpx;
  
void setup() 
{ 
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(3000);
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);  // GRB ordering is assumed
  FastLED.setBrightness(BRIGHTNESS);
  pinMode(LED_BUILTIN, OUTPUT);
  i=0;
  randomSeed(analogRead(A0));
}

void loop() 
{ 

  if (RAINBOW)
  {
    fill_rainbow(leds,NUM_LEDS,i++,1);
    FastLED.show();
  }
  else
  {
    randhue=random(0,255);
    pxidx=random(0,NUM_LEDS);
    leds[lastpx] = CRGB::Black;
    leds[pxidx].setHue(randhue);
    FastLED.show();
    delay(50);
    lastpx=pxidx;
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
  }
}
