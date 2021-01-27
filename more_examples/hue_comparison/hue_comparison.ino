//========================================================================================
//
//	SubPixel Example: Basic usage
//
//
//----------------------------------------------------------------------------------------
//
//
//========================================================================================

#include <Adafruit_NeoPixel.h>
#include <TheVaporTrail_SubPixel.h>

#define LED_PIN    2
#define LED_COUNT  144
#define LED_TYPE  (NEO_GRB + NEO_KHZ800)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, LED_TYPE);

#define PRECISION 4
#define SETTINGS (SUBPIX_OP_ADD + SUBPIX_WRAP)
TheVaporTrail_SubPixel stripSP = TheVaporTrail_SubPixel(PRECISION, SETTINGS, &strip);

void setup()
{
	strip.begin();
}

void loop()
{
  static uint16_t one = stripSP.one();
	static uint16_t delayTime = 300/one;
	static uint16_t startOffset = 0;
  static uint32_t spLength = stripSP.length();
  static uint32_t mask = stripSP.fractionMask();

  uint16_t location;
  uint16_t loc2;
  uint16_t hue = 0;
	
	strip.clear();
  
  location = startOffset;

  for (uint8_t i = 0; i < LED_COUNT/2 - 5; i++)
  {
    uint32_t clr = ColorHue(hue, LED_COUNT/2 - 5, 0x20);

    stripSP.setColor(location, clr, one);

    loc2 = location + one * LED_COUNT/2;
    loc2 = (loc2 & ~mask);
    stripSP.setColor(loc2, clr, one);

    location += one;
    hue++;
  }
  
 	strip.show();

	startOffset = (startOffset + 1) % spLength;

	delay(delayTime);
}

/*
uint32_t Hue(uint16_t hue, uint16_t range = 0xff, uint8_t brightness = 0xff)
{
  uint16_t phase = range/3;
  uint16_t r, g, b;

  hue = hue % range;
  
  if (hue < phase)
  {
    r = phase - hue;
    g = 0;
    b = hue;
  }
  else if (hue < 2*phase)
  {
    hue -= phase;
    r = 0;
    g = hue;
    b = phase - hue;
  }
  else
  {
    hue -= 2*phase;
    r = hue;
    g = phase - hue;
    b = 0;
  }

  r = ((uint32_t)r * brightness)/phase;
  g = ((uint32_t)g * brightness)/phase;
  b = ((uint32_t)b * brightness)/phase;

  return (uint32_t)r << 16 | (uint32_t)g << 8 | b;
}
*/
