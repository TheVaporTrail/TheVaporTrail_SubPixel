# TheVaporTrail_SubPixel

The **SubPixel** library enables smooth animations on LED strips and other LED collections by implementing anti-aliasing and color blending.

## Description

**SubPixel** introduces the concept of _sub-pixel addressing_ and _fractional LEDs_. For example, SubPixel can turn on one-quarter of one LED and three-quarters of the next LED with one call to `setColor()`. Called in a loop, SubPixel handles smoothly transitioning lighting one LED to the next. 

SubPixel supports color blending. If you call `setColor()` with red and then later with blue, the resulting LED color will be purple. While this might seem unusual, it allows for more natural-looking color animations, especially as different colors collide.

SubPixel in initialized with a **precision**, which specifies how many bits to use for _fixed-point_ values. Fixed-point values are used to specify fractional LED sizes and sub-pixel addresses. The precision can be zero, which results in only the blending capabilities being used. (A fixed-point value is basically a floating-point value multiplied by a number, such as 16, and stored in an integer).


## Installation
### Arduino
+ Download as a zip file.
+ Select Sketch > Include Library > Add .ZIP Library in the Arduino application

## Examples
+ **basic_neopixel**: Moves a single LED along an Adafruit_NeoPixel strip.
+ **basic_custom_pwm**: Moves a single non-RGB LED using the Arduino PWM GPIO pins.
+ **comparison**: Compares the smoothness of the transitions with different precision values
+ **hues**: Slowly moves a sequence of colors along the strip
+ **blend**: Demonstrates color blending
### More examples
+ **basic_custom_fastled**: Demonstrates using `TheVaporTrail_SubPixel_Params` constructor to work with the `FastLED` LED data
+ **basic_custom_neopixel**: Demonstrates using `TheVaporTrail_SubPixel_Params` constructor to work with an `Adafruit_NeoPixel` strip
+ **hue_comparison**: Comparison of smooth and non-smooth moving of rainbow colors.

## Initialization
There are three parameters to the SubPixel constructor
```c
TheVaporTrail_SubPixel(uint8_t precision, uint8_t flags, TheVaporTrail_SubPixel_Params* params);
TheVaporTrail_SubPixel(uint8_t precision, uint8_t flags, Adafruit_NeoPixel* neopixels);
```
### Precision
The _precision_ parameter specifies how many bits are used for the fixed-point. Valid values are from 0 to 7. 
### Settings
* Blend mode: SubPixel combines the new color specified in `setColor` with the current color in one of three ways. 
  + `SUBPIX_ADD`: The values in each of the color channels (red, green, and blue) are added together, but do not exceed 0xff. This usually results in the most realistic output. _(default)_
  + `SUBPIX_MAX`: The maximum of each color channel is used.
  + `SUBPIX_COPY`: The new color is copied into the LED, ignoring any previous color. (This mode is not recommended.)
* Wrap
  + `SUBPIX_NOWRAP`: Ignore LED addresses that are out of range. _(default)_
  + `SUBPIX_WRAP`: Wrap LED addresses the extend past the end of the LED strip around to the beginning. 
### LED Info
* NeoPixel: Pass an Adafruit_NeoPixel object, with `&`
```c
#define PRECISION 4
#define SETTINGS (SUBPIX_OP_ADD + SUBPIX_WRAP)
TheVaporTrail_SubPixel stripSP = TheVaporTrail_SubPixel(PRECISION, SETTINGS, &strip);
```
* Custom: Pass a TheVaporTrail_SubPixelFuncs, with `&`
```c
// Fill the structure with the LED count, the 'get function', and the 'set function'
// The last parameter is not used above, so we can pass NULL
TheVaporTrail_SubPixelFuncs funcs = { LED_COUNT, ledGetLevel, ledSetLevel, NULL };

// The precision, in bits
#define PRECISION 4
// Settings
#define SETTINGS (SUBPIX_OP_ADD + SUBPIX_WRAP)
// Initialize the SubPixel object
TheVaporTrail_SubPixel stripSP = TheVaporTrail_SubPixel(PRECISION, SETTINGS, &funcs);
```

## Usage
The primary call is `setColor()`. It takes three parameters
* location: a fixed-point offset into the LED strip.
* color: a 32-bit integer, where the low byte is green, the next byte is blue, and the third byte is red.
* width: a fixed-point value representing how many pixels to set to the color. 

(SubPixel does not have `clear` or `show` functions. You must call the AdaFruit_NeoPixel functions or other functions if not using NeoPixels.)

In this code `strip` is an Adafruit_NeoPixel object and `stripSP` is a TheVaporTrail_SubPixel object. The value `one` is equal to `1 << PRECISION` and is available by calling `stripSP.one()`

```c
  strip.clear();

  stripSP.setColor(offset, 0x400000, one);

  strip.show();
```

