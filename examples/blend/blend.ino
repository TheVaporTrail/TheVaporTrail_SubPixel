//========================================================================================
//
// SubPixel example: Blend
//
//   Demonstrates color blending
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
#define PRECISION 4
// Settings
#define SETTINGS (SUBPIX_OP_ADD + SUBPIX_WRAP)
// Initialize the SubPixel object
TheVaporTrail_SubPixel stripSP = TheVaporTrail_SubPixel(PRECISION, SETTINGS, &strip);

//----------------------------------------
// Animation values
//----------------------------------------
// Convenient constants for the primary colors.
// Note that these are not full brightness
#define RED   0x400000
#define GREEN 0x002000
#define BLUE  0x000040
// Time to move one full LED width
#define LED_INTERVAL 250
// "Width" of LEDs to color
#define LED_WIDTH 2

//----------------------------------------
// Arduino set-up
//----------------------------------------
void setup()
{
  strip.begin();
}

//----------------------------------------
// Arduino loop
//----------------------------------------
void loop()
{
  // Animation constants
  static uint16_t one = stripSP.one(); // Value equal to fixed-point 1.0, equals 1 << PRECISION
  static uint16_t delayTime = LED_INTERVAL/one; // Compute delay to keep the overall speed the same when changing precision (above)
  static uint16_t width = LED_WIDTH * one; // The width of the LEDs to set to a color
  static uint16_t spacing = LED_COUNT/3 * one; // The distance between the red, green, and blue colors
  static uint32_t spLength = stripSP.length(); // The length of the strip as a fixed-point value. 

  // Animation values, retained in calls to loop()
  static uint16_t fwdOffset = 0;
  static uint16_t revOffset = 0;

  // Animation values
  uint16_t location;

  // Clear the colors
  strip.clear();
  
  // Starting at the forward offset, set the RED color
  location = fwdOffset;
  stripSP.setColor(location, RED, width);
  
  // Advance by a third of the LEDs, set the GREEN color
  location += spacing;
  stripSP.setColor(location, GREEN, width);

  // Advance by another third of the LEDs, set the BLUE color
  location += spacing;
  stripSP.setColor(location, BLUE, width);

  // Starting at the reverse offset, set the RED color
  // Note that we are subtracting the revOffset to get these
  // values to move backword.
  location = 2 * spLength - 1 - revOffset;
  stripSP.setColor(location, RED, width);

  // Move back by a third of the LEDs, set the BLUE color
  location -= spacing;
  stripSP.setColor(location, BLUE, width);
  
  // Move back by another third of the LEDs, set the GREEN color
  location -= spacing;
  stripSP.setColor(location, GREEN, width);

  // Show the colors
  strip.show();

  // Advance the forward value every time.
  fwdOffset = (fwdOffset + 1) % spLength;

  // Advance the reverse value every other time.
  if (fwdOffset % 2)
    revOffset = (revOffset + 1) % spLength;

  delay(delayTime);
}
