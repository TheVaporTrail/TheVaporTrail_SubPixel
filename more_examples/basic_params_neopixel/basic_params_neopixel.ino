//========================================================================================
//
// SubPixel example: basic_params_neopixel
//
//   Usage with the TheVaporTrail_SubPixel_Params to specify getPixel and setPixel 
//   functions to access the Adafruit_Neopixel strip. 
//
//   This approach can be used on non-Arduino platforms or to intercept the values
//   provided to and read from the Adafruit_Neopixel API
//
//----------------------------------------------------------------------------------------
// (c) 2021 David Kaufman
// Released under the MIT license
// https://github.com/TheVaporTrail/TheVaporTrail_SubPixel
//========================================================================================

#include <TheVaporTrail_SubPixel.h>
#include <Adafruit_NeoPixel.h>

//----------------------------------------
// Adafruit NeoPixel initialization
//----------------------------------------
#define LED_PIN    2
#define LED_COUNT  32
#define LED_TYPE  (NEO_GRB + NEO_KHZ800)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, LED_TYPE);

//----------------------------------------
// Get Pixel Color
//   Called from SubPixel to get the pixel 
//   color. The context was passed to 
//   SubPixel in the constructor.
//----------------------------------------
uint32_t getPixelColor(void* context, uint16_t location)
{
  Adafruit_NeoPixel* neopixels = (Adafruit_NeoPixel*)context;
  return neopixels->getPixelColor(location);
}

//----------------------------------------
// Set Pixel Color
//   Called from SubPixel to set the pixel color
//----------------------------------------
void setPixelColor(void* context, uint16_t location, uint32_t color)
{
  Adafruit_NeoPixel* neopixels = (Adafruit_NeoPixel*)context;
  neopixels->setPixelColor(location, color);
}

//----------------------------------------
// SubPixel initialization
//----------------------------------------
// Fill the structure with the LED count, the 'get function', the 'set function',
// and a pointer the Adafruit_NeoPixel object
TheVaporTrail_SubPixel_Params params = { LED_COUNT, getPixelColor, setPixelColor, &strip };
// The precision, in bits
#define PRECISION 7
// Settings
#define SETTINGS (SUBPIX_OP_ADD + SUBPIX_WRAP)
// Initialize the SubPixel object
TheVaporTrail_SubPixel stripSP = TheVaporTrail_SubPixel(PRECISION, SETTINGS, &params);

//----------------------------------------
// Animation values
//----------------------------------------
// The color to set
#define COLOR 0x001010
// Time to move one full LED width
#define LED_INTERVAL 800

//----------------------------------------
// Arduino Set-up
//----------------------------------------
void setup()
{
  strip.begin();
}

//----------------------------------------
// Arduino Loop
//----------------------------------------
void loop()
{
  // Animation constants
  static uint16_t one = stripSP.one();
  static uint16_t delayTime = LED_INTERVAL/one;
  static uint32_t spLength = stripSP.length();
  
  // Animation values, values retained in calls to loop()
  static uint16_t offset = 0;

  strip.clear();
  
  stripSP.setColor(offset, COLOR, one);
  
  strip.show();

  offset = (offset + 1) % spLength;

  delay(delayTime);
}
