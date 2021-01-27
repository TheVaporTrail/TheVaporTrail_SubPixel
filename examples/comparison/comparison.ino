//========================================================================================
//
// SubPixel example: Comparison
//
//   Compares the smoothness of the transitions with different precision values
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
// SubPixel initialization
//----------------------------------------
// The precision, in bits
#define PRECISION 5
// Settings
#define SETTINGS (SUBPIX_OP_ADD + SUBPIX_WRAP)
// Initialize the SubPixel object
TheVaporTrail_SubPixel stripSP = TheVaporTrail_SubPixel(PRECISION, SETTINGS, &strip);

//----------------------------------------
// Animation values
//----------------------------------------
// Color to show. Note that this is not full brightness
#define COLOR 0x400000
// Time to offset one full LED
#define LED_INTERVAL 1200
// "Width" of LEDs to set
#define LED_WIDTH 1

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
  static uint16_t spacing = 4 * one;
  static uint16_t width = LED_WIDTH * one;
  static uint32_t spLength = stripSP.length();
  
  // Animation values, values retained in calls to loop()
  static uint16_t offset = 0;

  // Animation values
  uint16_t location = offset;
  uint16_t mask = stripSP.fractionMask();
  
  // Clear the strip
  strip.clear();
  
  // Set the color, rounding the position for each iteration  
  for (uint8_t i = 0; i <= PRECISION; i++)
  {
    uint16_t rounded = (location & ~mask);

    stripSP.setColor(rounded, COLOR, width);

    location += spacing;

    mask = (mask >> 1);
  }
  
  // Show the colors
  strip.show();

  // Increment the offset, wrapping if we reach the end of the sub-pixel length
  offset = (offset + 1) % spLength;

  // Small delay
  delay(delayTime);
}
