# WS2812FX-idf
WS2812FX with NeoPixelBus as Backend - working on ESP-IDF

First add NeoPixelBus-idf as a `component`. 

Add this. Note the `Adafruit_NeoPixel.h` is the link between `WS2812FX` and `NeoPixelBus`; it actually doesn't use Adafruit_NeoPixel at all.
