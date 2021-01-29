//=================================================================================
//
//	TheVaporTrail_SubPixel.cpp
//
//	Created by David Kaufman
//		0.1.0: January 20, 2021
//
//=================================================================================

#include <TheVaporTrail_SubPixel.h>

#define SUBPIX_OP_MASK	(0x3 << 1)


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

TheVaporTrail_SubPixel::TheVaporTrail_SubPixel(uint8_t precision, uint8_t settings, TheVaporTrail_SubPixel_Params* params)
{
	init(precision, settings);
	_params = params;
}

#ifdef ARDUINO
TheVaporTrail_SubPixel::TheVaporTrail_SubPixel(uint8_t precision, uint8_t settings, Adafruit_NeoPixel* neopixels)
{
	init(precision, settings);
	_neopixels = neopixels;
}
#endif

TheVaporTrail_SubPixel::~TheVaporTrail_SubPixel(void)
{
}

uint32_t TheVaporTrail_SubPixel::length(void)
{
	return (getPixelCount() << _precision);
}


void TheVaporTrail_SubPixel::setColor(uint32_t location, uint32_t color, uint16_t width)
{
	uint32_t srcColor, dstColor;
	uint8_t  amount;
	uint16_t loc    = (location >> _precision);
	uint8_t  remain = _one - (location & _mask);
	uint16_t count = getPixelCount();
	
	while (width > 0)
	{
		if (_settings & SUBPIX_WRAP)
			loc = loc % count;
			
		amount = (width < remain) ? width : remain;
		width -= amount;
		
		srcColor = scaleColor(color, amount);
		
		dstColor = getPixelColor(loc);
	
		dstColor = blendColor(dstColor, srcColor, _blendMode);
		
		setPixelColor(loc, dstColor);
		
		remain = _one;
		loc++;
	}
}


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

void TheVaporTrail_SubPixel::setPixelColor(uint16_t location, uint32_t color)
{
	if (_params != NULL && _params->setPixelColor != NULL)
		_params->setPixelColor(_params->context, location, color);
	#ifdef ARDUINO
	else if (_neopixels != NULL)
		_neopixels->setPixelColor(location, color);
	#endif
}


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

