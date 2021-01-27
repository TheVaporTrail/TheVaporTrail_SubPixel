//=================================================================================
//
//	TheVaporTrail_SubPixel.h
//
//	Created by David Kaufman
//		0.1.0: January 20, 2021
//
//	(license?)
//
//	Future features:
//		double-buffer
//		gamma correction
//		sub-strip support
//		alpha blending
//
//=================================================================================

#ifndef THEVAPORTRAIL_SUBPIXEL_H
#define THEVAPORTRAIL_SUBPIXEL_H

#ifdef ARDUINO
	#include "Arduino.h"
	#include <Adafruit_NeoPixel.h>
#endif

#define SUBPIX_NOWRAP	(0x0 << 0)
#define SUBPIX_WRAP		(0x1 << 0)

#define SUBPIX_OP_ADD	(0x0 << 1)
#define SUBPIX_OP_MAX	(0x1 << 1)
#define SUBPIX_OP_COPY	(0x2 << 1)


typedef struct {
	uint16_t	pixelCount;
	uint32_t	(*getPixelColor)(void*, uint16_t);
	void		(*setPixelColor)(void*, uint16_t, uint32_t);
	void*		context;
} TheVaporTrail_SubPixel_Params;

class TheVaporTrail_SubPixel {

	public:

		// Constructor: precision, settings, 
		TheVaporTrail_SubPixel(uint8_t precision, uint8_t settings, TheVaporTrail_SubPixel_Params* params);
		#ifdef ARDUINO
		TheVaporTrail_SubPixel(uint8_t precision, uint8_t settings, Adafruit_NeoPixel* neopixels);
		#endif
	   ~TheVaporTrail_SubPixel(void);
	   
	    uint8_t precision(void) { return _precision; }
		uint16_t one(void) { return _one; }
		uint16_t fractionMask(void)  { return _mask; };
		uint32_t length(void);
		void setColor(uint32_t location, uint32_t color, uint16_t width);
		uint32_t scaleColor(uint32_t color, uint8_t fraction);
		uint32_t blendColor(uint32_t srcColor, uint32_t dstColor, uint8_t blendOp);
		
	private:
	
		uint8_t				_precision;
		uint16_t			_one;
		uint16_t			_mask;
		uint8_t				_settings;
		uint8_t				_blendMode;
		#ifdef ARDUINO
		Adafruit_NeoPixel*	_neopixels;
		#endif
		TheVaporTrail_SubPixel_Params* _params;
		
		void		init(uint8_t precision, uint8_t blendMode);
		uint16_t	getPixelCount(void);
		uint32_t	getPixelColor(uint16_t location);
		void		setPixelColor(uint16_t location, uint32_t color);
};


// Utilities
uint32_t ColorHue(uint16_t hue, uint16_t range = 0xff, uint8_t brightness = 0xff);


#endif // THEVAPORTRAIL_SUBPIXEL_H