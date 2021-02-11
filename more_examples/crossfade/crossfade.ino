//========================================================================================
//
// SubPixel example: Crossfade
//
//   Demonstrates using setAlpha to crossfade between two animations 
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
//   Smaller values result in faster animations
#define LED_INTERVAL 300
// Maximum brightness for the LEDs
#define LED_BRIGHTNESS 0x30
// Amount to change the alpha at each step during crossfade
//   Smaller values result in longer crossfade
#define DELTA_ALPHA 0x10
// Amount of steps to show each animation before fading
//   Larger values hold the animation longer
#define HOLD_STEPS 70

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

  // Crossfade values, values retained in calls to loop()
  static uint8_t alpha = 0;
  static int8_t alphaState = 1; // 0: fade (down); 1: hold-then-up, 2: face (up), 3: hold-then-down;
  static uint16_t alphaCounter = HOLD_STEPS;

  // Clear the strip
  strip.clear();

  // Set the alpha and draw the first animation
  // If the alpha is zero, then none of the animation is applied.
  stripSP.setAlpha(alpha);
  hues();
   
  // Set the inverse alpha and draw the second animation
  // If alpha is 0xff, then none of the animation is applied
  stripSP.setAlpha(0xff - alpha);
  blends();
   
  // Show the colors
  strip.show();

  // Based on the state we are in, adjust either the alpha value or the counter
  //
  if (alphaState == 0) // Fade (down)
  {
    if (alpha < DELTA_ALPHA)
    {
      alpha = 0;
      alphaState = 1;
      alphaCounter = HOLD_STEPS;
    }
    else
    {
      alpha -= DELTA_ALPHA;
    }
  }
  else if (alphaState == 1) // Hold
  {
    alphaCounter--;
    if (alphaCounter == 0)
      alphaState = 2;
  }
  else if (alphaState == 2) // Fade (up)
  {
    if (alpha > 0xFF - DELTA_ALPHA)
    {
      alpha = 0xff;
      alphaState = 3;
      alphaCounter = HOLD_STEPS;
    }
    else
    {
      alpha += DELTA_ALPHA;
    }
  }
  else // Hold
  {
    alphaCounter--;
    if (alphaCounter == 0)
      alphaState = 0;
  }

  // Small delay
  delay(delayTime);
}

//----------------------------------------
// Animation: Hues
//----------------------------------------
void hues(void)
{
  // Animation constants
  static uint16_t one = stripSP.one();
  static uint32_t spLength = stripSP.length();

  // Animation values, values retained in calls to loop()
  static uint16_t startOffset = 0;

  // Animation values
  uint16_t location = startOffset;
  uint16_t hue = 0;
 
  // Set a color for each LED
  for (uint8_t i = 0; i < LED_COUNT; i++)
  {
    uint32_t clr = ColorHue(hue, LED_COUNT, LED_BRIGHTNESS);

    stripSP.setColor(location, clr, one);

    location += one;
    hue++;
  }
  // Increment the offset, wrapping if we reach the end of the sub-pixel length
  // Note that we are incrementing a fixed-point value. The increment is equivalent
  // to 1/(2^PRECISION)
  startOffset = (startOffset + 3) % spLength;

}

//----------------------------------------
// Animation: Blends
//----------------------------------------
// Convenient constants for the primary colors.
#define RED   ((uint32_t)LED_BRIGHTNESS << 16)
#define GREEN ((uint32_t)LED_BRIGHTNESS <<  8)
#define BLUE  ((uint32_t)LED_BRIGHTNESS      )

// "Width" of LEDs to color
#define LED_WIDTH 2

void blends(void)
{
  // Animation constants
  static uint16_t one = stripSP.one();
  static uint16_t width = LED_WIDTH * one;
  static uint16_t spacing = LED_COUNT/3 * one; // The distance between the red, green, and blue colors
  static uint32_t spLength = stripSP.length(); // The length of the strip as a fixed-point value. 

  // Animation values, retained in calls to loop()
  static uint16_t fwdOffset = 0;
  static uint16_t revOffset = 0;

  // Animation values
  uint16_t location;
  
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

  // Advance the forward value every time.
  fwdOffset = (fwdOffset + 5) % spLength;

  // Advance the reverse value every other time.
  if (fwdOffset % 2)
    revOffset = (revOffset + 5) % spLength;

}
