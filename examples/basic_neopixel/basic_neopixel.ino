//========================================================================================
//
// SubPixel example: Basic Neopixel
//
//   Usage with Adafruit_NeoPixel library
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
  static uint16_t one = stripSP.one(); // Value equal to fixed-point 1.0, equals 1 << PRECISION or 2^PRECISION
  static uint16_t delayTime = LED_INTERVAL/one; // Compute delay to keep the overall speed the same when changing precision (above)
  static uint32_t spLength = stripSP.length(); // The length of the strip as a fixed-point value. 

  // Animation values, values retained in calls to loop()
  static uint16_t offset = 0; // Fixed-point value

  // Clear the strip.
  strip.clear();

  // Set the color. Note that offset is fixed-point, so will spread the
  // color across multiple LEDs (except when the offset is pointing to the 
  // beginning of an LED).
  stripSP.setColor(offset, COLOR, one);

  // Show the color
  strip.show();

  // Advance the offset. Since offset if a fixed-point number, this increases
  // the offset by a fraction equal to 1/(2^PRECISION). Also use the spLength to
  // keep the offset in range.
  offset = (offset + 1) % spLength;
 
  // Small delay
  delay(delayTime);
}
