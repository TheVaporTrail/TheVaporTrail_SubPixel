//========================================================================================
//
// SubPixel example: basic_params_fastled
//
//   Using FastLED with TheVaporTrail_SubPixel_Params. Demonstrates using the
//   TheVaporTrail_SubPixel_Params to specify getPixel and setPixel 
//   functions to access the read and write from the LED data array. 
//
//   This approach can be used on non-Arduino platforms or to intercept the values
//   provided to and read from the LED data
//
//----------------------------------------------------------------------------------------
// (c) 2021 David Kaufman
// Released under the MIT license
// https://github.com/TheVaporTrail/TheVaporTrail_SubPixel
//========================================================================================

#include <TheVaporTrail_SubPixel.h>
#include <FastLED.h>

//----------------------------------------
// FastLED configuration and LED data
//----------------------------------------
#define LED_PIN    3
#define LED_COUNT  32

CRGB ledData[LED_COUNT];

//----------------------------------------
// Get Pixel Color
//   Called from SubPixel to get the pixel color.
//----------------------------------------
uint32_t getPixelColor(void* context, uint16_t location)
{
  return ledData[location];
}

//----------------------------------------
// Set Pixel Color
//   Called from SubPixel to set the pixel color
//----------------------------------------
void setPixelColor(void* context, uint16_t location, uint32_t color)
{
  ledData[location] = color;
}

//----------------------------------------
// SubPixel initialization
//----------------------------------------
// Fill the params with the LED count, the 'get function', and the 'set function'
TheVaporTrail_SubPixel_Params params = { LED_COUNT, getPixelColor, setPixelColor, NULL };
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
#define COLOR 0x004040
// Time to move one full LED width
#define LED_INTERVAL 1000

//----------------------------------------
// Arduino Set-up
//----------------------------------------
void setup()
{
  FastLED.addLeds<NEOPIXEL, LED_PIN>(ledData, LED_COUNT);
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

  FastLED.clear();
  
  stripSP.setColor(offset, COLOR, 3*one);
  
  FastLED.show();

  offset = (offset + 1) % spLength;

  delay(delayTime);
}
