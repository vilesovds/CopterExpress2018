#include <FastLED.h>

FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few 
// of the kinds of animation patterns you can quickly and easily 
// compose using FastLED.  
//
// This example also shows one easy way to define multiple 
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    3
//#define CLK_PIN   4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    32
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          80
#define FRAMES_PER_SECOND  120

void setup() {
  delay(3000); // 3 second delay for recovery
  Serial.begin(115200);
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow,rainbowWithGlitter, /*confetti,*/ sinelon,/* juggle,*/ bpm, dummy };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
  
void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  if(Serial.available()){
    String req = Serial.readStringUntil('\n');
    if(req.equalsIgnoreCase("rainbow")){
       gCurrentPatternNumber = 0;
    }else if(req.equalsIgnoreCase("rainbowWithGlitter")){
       gCurrentPatternNumber = 1;
    }else if(req.equalsIgnoreCase("sinelon")){
       gCurrentPatternNumber = 2;
    }else if(req.equalsIgnoreCase("bpm")){
       gCurrentPatternNumber = 3;
    }else if (req.startsWith("all(")){
      // look for the next valid integer in the incoming serial stream:
      int red,green, blue;
      sscanf(req.c_str(),"all(%d,%d,%d)",&red,&green,&blue);
      all(red&0xFF,green&0xFF,blue&0xFF);
      //all(255,100,100);
      gCurrentPatternNumber = 4;
    }else if (req.startsWith("any(")){
      // look for the next valid integer in the incoming serial stream:
      int led,red,green, blue;
      sscanf(req.c_str(),"any(%d,%d,%d,%d)",&led,&red,&green,&blue);
      any(led%NUM_LEDS,red&0xFF,green&0xFF,blue&0xFF);
      gCurrentPatternNumber = 4;
      
    }else if (req.startsWith("one(")){
      // look for the next valid integer in the incoming serial stream:
      int led,red,green, blue;
      sscanf(req.c_str(),"one(%d,%d,%d,%d)",&led,&red,&green,&blue);
      one(led%NUM_LEDS,red&0xFF,green&0xFF,blue&0xFF);
      gCurrentPatternNumber = 4;
    }else if (req.startsWith("getLedsCount")){
      Serial.print(NUM_LEDS);
      Serial.write('\n');
    }
    
  }else{
    // insert a delay to keep the framerate modest
    FastLED.delay(1000/FRAMES_PER_SECOND); 
  }

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  //EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 26, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void all(int r, int g, int b)
{
    fill_solid(leds, NUM_LEDS, CRGB(r,g,b));
}

void any(int led, int r, int g, int b)
{
    leds[led] =  CRGB(r,g,b);
}

void one(int led, int r, int g, int b)
{
    fill_solid(leds, NUM_LEDS, CRGB(0,0,0));
    leds[led] =  CRGB(r,g,b);
}


void dummy()
{
}
