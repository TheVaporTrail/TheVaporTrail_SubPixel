//========================================================================================
//
// SubPixel example: Hues
//
//   Demonstrates the smooth animation of moving and blending a full range of colors 
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
#define PRECISION 3
// Settings
#define SETTINGS (SUBPIX_OP_ADD + SUBPIX_WRAP)
// Initialize the SubPixel object
TheVaporTrail_SubPixel stripSP = TheVaporTrail_SubPixel(PRECISION, SETTINGS, &strip);

//----------------------------------------
// Animation values
//----------------------------------------
// Time to offset one full LED
#define LED_INTERVAL 300
// Maximum brightness for the LEDs
#define LED_BRIGHTNESS 0x20

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
  static uint16_t startOffset = 0;

  // Animation values
  uint16_t location = startOffset;
  uint16_t hue = 0;

  // Clear the strip
  strip.clear();
  
  // Set a color for each LED
  for (uint8_t i = 0; i < LED_COUNT; i++)
  {
    uint32_t clr = ColorHue(hue, LED_COUNT, LED_BRIGHTNESS);

    stripSP.setColor(location, clr, one);

    location += one;
    hue++;
  }
  
  // Show the colors
  strip.show();

  // Increment the offset, wrapping if we reach the end of the sub-pixel length
  // Note that we are incrementing a fixed-point value. The increment is equivalent
  // to 1/(2^PRECISION)
  startOffset = (startOffset + 1) % spLength;

  // Small delay
  delay(delayTime);
}

