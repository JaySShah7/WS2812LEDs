/*
 * Arduino interface for the use of WS2812 strip LEDs * The 'Magic word' is 'Ada'
 * Uses the FASTLED v3.001 Library
 */
#include "FastLED.h"
#define NUM_LEDS 30
#define DATA_PIN 6
#define BRIGHTNESS 255
#define SATURATION 1.2
// Baudrate, higher rate allows faster refresh rate and more LEDs (defined in /etc/boblight.conf)
#define serialRate 115200

// Adalight sends a "Magic Word" (defined in /etc/boblight.conf) before sending the pixel data
uint8_t prefix[] = {'A', 'd', 'a'}, hi, lo, chk, i;

// Initialise LED-array
CRGB leds[NUM_LEDS];

void setup() {
  FastLED.setBrightness(BRIGHTNESS);
  // Use NEOPIXEL to keep true colors
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  
  // Initial RGB flash

  for(i=0;i<254;++i){
LEDS.showColor(CRGB(i,i,i));
 delay(1);}
  for(i=255;i>0;--i){
LEDS.showColor(CRGB(i,i,i));
 delay(2);}
   for(i=0;i<254;++i){
LEDS.showColor(CRGB(i,0,0));
 delay(1);}
  for(i=255;i>0;--i){
LEDS.showColor(CRGB(i,0,0));
 delay(2);}
   for(i=0;i<254;++i){
LEDS.showColor(CRGB(0,i,0));
 delay(1);}
  for(i=255;i>0;--i){
LEDS.showColor(CRGB(0,i,0));
 delay(2);}
   for(i=0;i<254;++i){
LEDS.showColor(CRGB(0,0,i));
 delay(1);}
  for(i=255;i>0;--i){
LEDS.showColor(CRGB(0,0,i));
 delay(2);}
LEDS.showColor(CRGB(0,0,0));

 
 //begin serial 
  Serial.begin(serialRate);
  // Send "Magic Word" string to host
  Serial.print("Ada\n");
}

void loop() { 
  // Wait for first byte of Magic Word
  for(i = 0; i < sizeof prefix; ++i) {
    waitLoop: while (!Serial.available()) ;;
    // Check next byte in Magic Word
    if(prefix[i] == Serial.read()) continue;
    // otherwise, start over
    i = 0;
    goto waitLoop;
  }
  
  // Hi, Lo, Checksum  
  while (!Serial.available()) ;;
  hi=Serial.read();
  while (!Serial.available()) ;;
  lo=Serial.read();
  while (!Serial.available()) ;;
  chk=Serial.read();
  
  // If checksum does not match go back to wait
  if (chk != (hi ^ lo ^ 0x55)) {
    i=0;
    goto waitLoop;
  }
  
  memset(leds, 0, NUM_LEDS * sizeof(struct CRGB));
  // Read the transmission data and set LED values
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    byte r, g, b;    
    while(!Serial.available());
    r = Serial.read();
    while(!Serial.available());
    g = Serial.read();
    while(!Serial.available());
    b = Serial.read();
    if (r>g && r>b)   {b=b/SATURATION;g=g/SATURATION;}
    else if (g>r&&g>b){r=r/SATURATION;b=b/SATURATION;}
    else if (b>r&&b>g){r=r/SATURATION;g=g/SATURATION;}
    leds[i].r = r;
    leds[i].g = g;
    leds[i].b = b;
  }
  
  // Shows new values

  FastLED.show();
}
