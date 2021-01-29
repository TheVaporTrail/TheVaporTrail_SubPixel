//=================================================================================
//
//	TheVaporTrail_SubPixel.cpp
//
//----------------------------------------------------------------------------------------
// (c) 2021 David Kaufman
// Released under the MIT license
// https://github.com/TheVaporTrail/TheVaporTrail_SubPixel
//========================================================================================

#include <TheVaporTrail_SubPixel.h>

#define SUBPIX_OP_MASK	(0x3 << 1)


//---------------------------------------------------------------------------------
//	Init
//		common initialization for the constructors
//---------------------------------------------------------------------------------
void TheVaporTrail_SubPixel::init(uint8_t precision, uint8_t settings)
{
	_precision = precision;
	_settings = settings;
	_blendMode = (_settings & SUBPIX_OP_MASK);
	_params = NULL;
	#ifdef ARDUINO
	_neopixels = NULL;
	#endif
	
	_one  = (1 << _precision);
	_mask = _one - 1;
}

//---------------------------------------------------------------------------------
//	constructor with TheVaporTrail_SubPixel_Params
//---------------------------------------------------------------------------------
TheVaporTrail_SubPixel::TheVaporTrail_SubPixel(uint8_t precision, uint8_t settings, TheVaporTrail_SubPixel_Params* params)
{
	init(precision, settings);
	_params = params;
}

#ifdef ARDUINO
//---------------------------------------------------------------------------------
//	constructor with Adafruit_NeoPixel
//---------------------------------------------------------------------------------
TheVaporTrail_SubPixel::TheVaporTrail_SubPixel(uint8_t precision, uint8_t settings, Adafruit_NeoPixel* neopixels)
{
	init(precision, settings);
	_neopixels = neopixels;
}
#endif

//---------------------------------------------------------------------------------
//	destructor
//---------------------------------------------------------------------------------
TheVaporTrail_SubPixel::~TheVaporTrail_SubPixel(void)
{
}

//---------------------------------------------------------------------------------
//	Length
//		Returns the length as a fixed-point value
//---------------------------------------------------------------------------------
uint32_t TheVaporTrail_SubPixel::length(void)
{
	return (getPixelCount() << _precision);
}

//---------------------------------------------------------------------------------
//	Set Color
//---------------------------------------------------------------------------------
void TheVaporTrail_SubPixel::setColor(uint32_t location, uint32_t color, uint16_t width)
{
	uint32_t srcColor, dstColor;
	uint8_t  amount;
	// Convert the fixed-point location to an integer position in the LED strip
	uint16_t loc    = (location >> _precision);
	// Determine how much of the current position is available to fill. This is
	// the distance from the sub-pixel location to the next integer location 
	uint8_t  remain = _one - (location & _mask);
	// The LED count is used for wrapping the position
	uint16_t count = getPixelCount();
	
	// Loop as long as their is "width" remaining to fill into the LEDs
	while (width > 0)
	{
		if (_settings & SUBPIX_WRAP)
			loc = loc % count;
		
		// Determine how much of the current LED to fill with the color
		amount = (width < remain) ? width : remain;
		
		// And remove that amount from the width.
		width -= amount;
		
		// Scale (adjust the brightness of) the color according to how much
		// of the LED we are going to fill
		srcColor = scaleColor(color, amount);
		
		// Get the current color in the pixel, to use for blending
		dstColor = getPixelColor(loc);
	
		// Blend the new color with the color currently in the pixel.
		dstColor = blendColor(dstColor, srcColor, _blendMode);
		
		// Update the pixel color
		setPixelColor(loc, dstColor);
		
		// After the first LED, we now can fill up to the entire LED.
		remain = _one;
		
		// Move to the next LED in the strip
		loc++;
	}
}

//---------------------------------------------------------------------------------
//	Scale Color
//---------------------------------------------------------------------------------
uint32_t TheVaporTrail_SubPixel::scaleColor(uint32_t color, uint8_t fraction)
{
	uint32_t clr = color;

	if (fraction < _one)
	{
		clr  = ((((color >> 16) & 0xff) * fraction) / _one) << 16;
		clr |= ((((color >>  8) & 0xff) * fraction) / _one) <<  8;
		clr |= ((((color      ) & 0xff) * fraction) / _one);
	}

	return clr; 
}

//---------------------------------------------------------------------------------
//	Blend Color
//---------------------------------------------------------------------------------
uint32_t TheVaporTrail_SubPixel::blendColor(uint32_t dstColor, uint32_t srcColor, uint8_t blendOp)
{
	uint8_t r, g, b, c;

	r = (srcColor >> 16) & 0xff;
	c = (dstColor >> 16) & 0xff;
	
	if (blendOp == SUBPIX_OP_COPY)
		; // Use r as is
	else if (blendOp == SUBPIX_OP_MAX)
		r = (r > c) ? r : c;
	else // SUBPIX_OP_ADD
		r = (r < 0xff - c) ? (r + c) : 0xff;

	g = (srcColor >>  8) & 0xff;
	c = (dstColor >>  8) & 0xff;
	
	if (blendOp == SUBPIX_OP_COPY)
		; // Use g as is
	else if (blendOp == SUBPIX_OP_MAX)
		g = (g > c) ? g : c;
	else // SUBPIX_OP_ADD
		g = (g < 0xff - c) ? (g + c) : 0xff;

	b = (srcColor      ) & 0xff;
	c = (dstColor      ) & 0xff;

	if (blendOp == SUBPIX_OP_COPY)
		; // Use b as is
	else if (blendOp == SUBPIX_OP_MAX)
		b = (b > c) ? b : c;
	else // SUBPIX_OP_ADD
		b = (b < 0xff - c) ? (b + c) : 0xff;

	dstColor = ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;

	return dstColor;
}

//---------------------------------------------------------------------------------
//	Get Pixel Count
//---------------------------------------------------------------------------------
uint16_t TheVaporTrail_SubPixel::getPixelCount(void)
{
	uint32_t count;
	
	if (_params != NULL)
		count = _params->pixelCount;
	#ifdef ARDUINO
	else if (_neopixels != NULL)
		count = _neopixels->numPixels();
	#endif
	else
		count = 0;
	 
	
	return count;
}

//---------------------------------------------------------------------------------
//	Get Pixel Color
//		location: integer index into LED list
//---------------------------------------------------------------------------------
uint32_t TheVaporTrail_SubPixel::getPixelColor(uint16_t location)
{
	uint32_t color;
	
	if (_params != NULL && _params->getPixelColor != NULL)
		color = _params->getPixelColor(_params->context, location);
	#ifdef ARDUINO
	else if (_neopixels != NULL)
		color = _neopixels->getPixelColor(location);
	#endif
	else
		color = 0x000000;
		
	
	return color;
}

//---------------------------------------------------------------------------------
//	Set Pixel Color
//---------------------------------------------------------------------------------
void TheVaporTrail_SubPixel::setPixelColor(uint16_t location, uint32_t color)
{
	if (_params != NULL && _params->setPixelColor != NULL)
		_params->setPixelColor(_params->context, location, color);
	#ifdef ARDUINO
	else if (_neopixels != NULL)
		_neopixels->setPixelColor(location, color);
	#endif
}

//=================================================================================
//
//	Utilities
//
//=================================================================================

//---------------------------------------------------------------------------------
//	Color Hue
//---------------------------------------------------------------------------------
uint32_t ColorHue(uint16_t hue, uint16_t range /*= 0xff*/, uint8_t brightness /*= 0xff*/)
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
    if (hue > phase)
	    hue = phase;
    r = hue;
    g = phase - hue;
    b = 0;
  }

  r = ((uint32_t)r * brightness)/phase;
  g = ((uint32_t)g * brightness)/phase;
  b = ((uint32_t)b * brightness)/phase;

  return (uint32_t)r << 16 | (uint32_t)g << 8 | b;
}

