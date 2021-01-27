//========================================================================================
//
// SubPixel example: basic_custom_pwm
//
//   Usage with the TheVaporTrail_SubPixel_Params to specify custom getPixel 
//   and setPixel functions.
//
//----------------------------------------------------------------------------------------
// (c) 2021 David Kaufman
// Released under the MIT license
// https://github.com/TheVaporTrail/TheVaporTrail_SubPixel
//========================================================================================

#include <TheVaporTrail_SubPixel.h>

//----------------------------------------
// PWM Pins as a driver for multiple LEDs
//----------------------------------------
// List of Arduino PWM pins to drive LEDs
const uint8_t ledPWMPins[] = { 3, 5, 6, 9, 10, 11 };

// Get count of LEDs from the length of the array
#define LED_COUNT sizeof(ledPWMPins)/sizeof(*ledPWMPins)

// Data for the LED brightness
uint8_t ledLevels[LED_COUNT];

//----------------------------------------
// LED Set-up: Initialize the PWM pins as outputs
//----------------------------------------
void ledSetup(void)
{
  for (uint8_t i = 0; i < LED_COUNT; i++)
    pinMode(ledPWMPins[i], OUTPUT);
}

//----------------------------------------
// LED Clear All: Set the LEDs to off
//----------------------------------------
void ledClearAll(void)
{
  for (uint8_t i = 0; i < LED_COUNT; i++)
    ledSetLevel(NULL, i, 0);
}

//----------------------------------------
// LED Get Level: Get the LED brightness 
// from the array
//----------------------------------------
uint32_t ledGetLevel(void* context, uint16_t location)
{
  return (location < LED_COUNT) ? ledLevels[location] : 0;
}

//----------------------------------------
// LED Set Level: Set the LED brightness 
// in the array and into the PWM pin
//----------------------------------------
void ledSetLevel(void* context, uint16_t location, uint32_t color)
{
  if (location < LED_COUNT)
  {
    // Make it clear that we are using the lower byte of the color data
    uint8_t value = (color & 0xff);
    ledLevels[location] = value;
    analogWrite(ledPWMPins[location], value);
  }
}


//----------------------------------------
// SubPixel initialization
//----------------------------------------
// Fill the structure with the LED count, the 'get function', and the 'set function'
// The context, the last field, is not used above, so we can set it to NULL
TheVaporTrail_SubPixel_Params subpix_params = { LED_COUNT, ledGetLevel, ledSetLevel, NULL };
// The precision, in bits
#define PRECISION 4
// Settings
#define SETTINGS (SUBPIX_OP_ADD + SUBPIX_WRAP)
// Initialize the SubPixel object
TheVaporTrail_SubPixel stripSP = TheVaporTrail_SubPixel(PRECISION, SETTINGS, &subpix_params);


//----------------------------------------
// Animation values
//----------------------------------------
// Time to offset one full LED
#define LED_INTERVAL 800

//----------------------------------------
// Arduino Set-up
//----------------------------------------
void setup()
{
  // Set-up the LED pins
  ledSetup();
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
  
  // Clear the LEDs
  ledClearAll();
  
  // Set the color. Note that offset is fixed-point, so will spread the
  // color across multiple LEDs (except when the offset is pointing to the 
  // beginning of an LED).
  stripSP.setColor(offset, 0xff, one);

  // Advance the offset. Since offset if a fixed-point number, this increases
  // the offset by a fraction equal to 1/(2^PRECISION). Also use the spLength to
  // keep the offset in range.
  offset = (offset + 1) % spLength;

  // Small delay
  delay(delayTime);
}