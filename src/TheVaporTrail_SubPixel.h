//=================================================================================
//
//	TheVaporTrail_SubPixel.h
//
//----------------------------------------------------------------------------------------
// (c) 2021 David Kaufman
// Released under the MIT license
// https://github.com/TheVaporTrail/TheVaporTrail_SubPixel
//========================================================================================


#ifndef THEVAPORTRAIL_SUBPIXEL_H
#define THEVAPORTRAIL_SUBPIXEL_H

#ifdef ARDUINO
	#include "Arduino.h"
	// We assume that the Adafruit_NeoPixel library is available is we are 
	// compiling in the Arduino environment
	#include <Adafruit_NeoPixel.h>
#endif

// Wrap settings:
//	NOWRAP: LED locations out of range are ignored
//	WRAP: LED locations out of range are wrapped into range. This is useful especially
//		for circular arrangements of LEDs.
#define SUBPIX_NOWRAP	(0x0 << 0)
#define SUBPIX_WRAP		(0x1 << 0)

// Blend settings:
//	ADD: Add the the color channels together when (red + red, etc) when combining the
//		new pixel with the pixel already in the strip. This is generally the most natural
//		as it keeps the relative brightness the same (if possible).
#define SUBPIX_OP_ADD	(0x0 << 1)
//	MAX: Select the maximum of each color channel
#define SUBPIX_OP_MAX	(0x1 << 1)
//	COPY: Copy the new color pixel, replacing the current pixel. This is not recommended
//		since it may cause a new pixel to clear the results of a previous pixel if they
//		share an LED
#define SUBPIX_OP_COPY	(0x2 << 1)

// Parameters for providing a custom LED strip API
//	pixelCount: The number of pixels in the strip
//	getPixelColor: A function to get the color of a pixel
//	setPixelColor: A function to set the color of a pixel
//	context: An optional pointer that is passed to getPixelColor and setPixelColor
typedef struct {
	uint16_t	pixelCount;
	uint32_t	(*getPixelColor)(void*, uint16_t);
	void		(*setPixelColor)(void*, uint16_t, uint32_t);
	void*		context;
} TheVaporTrail_SubPixel_Params;

class TheVaporTrail_SubPixel {

	public:

		// Constructor
		//	precision: the number of bits for the fixed-point math. valid values are 0 to 7
		//		Reasonable values are 3 and 4. Value 0 is appropriate to use the blending 
		//		capabilities without the sub-pixel addressing
		//	settings: see the SUBPIX_ flags above
		TheVaporTrail_SubPixel(uint8_t precision, uint8_t settings, TheVaporTrail_SubPixel_Params* params);
		#ifdef ARDUINO
		TheVaporTrail_SubPixel(uint8_t precision, uint8_t settings, Adafruit_NeoPixel* neopixels);
		#endif
	   ~TheVaporTrail_SubPixel(void);
	   
		// Precision: returns the precision parameter passed in the constructor
	    uint8_t precision(void) { return _precision; }
	    
		// One: returns 1 << precision, which is equal to 2^precision
		uint16_t one(void) { return _one; }
		
		// FractionMask: returns a mask of the fractional bits of the location. 
		//		equal to 2^precision - 1
		uint16_t fractionMask(void)  { return _mask; };
		
		// Length: The length of the strip as a fixed-point value. Equal
		//		to the integer length * 2^precision
		uint32_t length(void);
		
		// Set Color: Set the color of one or more pixels
		//	location: fixed-point address 
		//	color: color as an RGB value
		//	width: fixed-point value specifying how much of the strip to fill
		//		use one() to fill a single pixel width.
		void setColor(uint32_t location, uint32_t color, uint16_t width);
		
		// Scale Color: scale each of the channels of a color
		uint32_t scaleColor(uint32_t color, uint8_t fraction, uint8_t alpha);
		
		// Blend two colors together according to the blendOp (see SUBPIX_OP_.., above)
		uint32_t blendColor(uint32_t srcColor, uint32_t dstColor, uint8_t blendOp);
		
		void setAlpha(uint8_t alpha);
		
	private:
	
		uint8_t				_precision;
		uint16_t			_one;
		uint16_t			_mask;
		uint8_t				_settings;
		uint8_t				_blendMode;
		uint8_t				_alpha;
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