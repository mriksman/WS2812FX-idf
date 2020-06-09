# WS2812FX-idf
WS2812FX with NeoPixelBus as Backend - working on ESP-IDF

First add NeoPixelBus-idf as a `component`. 

Add this. Note the `Adafruit_NeoPixel.h` is the link between `WS2812FX` and `NeoPixelBus`; it actually doesn't use Adafruit_NeoPixel at all.

Note; you don't need to set a 'Type' when declaring WS2812FX
``` c
WS2812FX ws2812fx = WS2812FX(PixelCount, PixelPin, 0);  //type is set in Adafruit_NeoPixel.h
```
In `Adafruit_NeoPixel.h`, set the NeoPixelBus up in there;
```cpp
typedef NeoGrbwFeature COLOR_FEATURE;
typedef NeoPixelBrightnessBus<COLOR_FEATURE, Neo800KbpsMethod> NPB_IMPL;
```
