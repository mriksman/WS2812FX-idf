/**
 * Port of WS2821FX to run under ESP8266_RTOS
 * Origin repo: https://github.com/kitesurfer1404/WS2812FX
 * 
 * minimize modifications on the original WS2812FX implementation
 */

#pragma once

#include <cstdint>
#include <cstdlib>

#include <NeoPixelBrightnessBus.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_system.h>

// old style flash access
#define PROGMEM
#define F(x)  (x)
typedef char __FlashStringHelper;

typedef uint16_t neoPixelType;
typedef bool     boolean;
typedef uint8_t  byte;
#define max      std::max
#define min      std::min

// Sort out Arduino binary defines...
#define B00000000 0b00000000
#define B00000010 0b00000010
#define B00010000 0b00010000
#define B00100000 0b00100000
#define B00110000 0b00110000
#define B01010000 0b01010000
#define B01100000 0b01100000
#define B01110000 0b01110000
#define B00001000 0b00001000
#define B00000100 0b00000100
#define B00000110 0b00000110
#define B10000000 0b10000000
#define B01000000 0b01000000


template<typename T>
T random() {
    return esp_random();
}

template<typename T>
T random(const T& maxVal) {
    return (random<T>() % maxVal);
}

// ** Modified to accept (uint16_t, int)
template<typename T, typename U>
T random(const T& minVal, const U& maxVal) {
    return minVal + random<T>((maxVal - minVal));
}


// http://arduino.cc/en/Reference/constrain
template<class T1, class T2, class T3>
const T1 constrain(const T1& x, const T2& a, const T3& b) {
    if (x < a)
        return a;
    else if (b < x)
        return b;
    else
        return x;
}

// typedef NeoPixelBus configuration
typedef NeoGrbwFeature COLOR_FEATURE;
typedef NeoPixelBrightnessBus<COLOR_FEATURE, Neo800KbpsMethod> NPB_IMPL;

// Adafruit_NeoPixel compatibility layer
// https://github.com/adafruit/Adafruit_NeoPixel
class Adafruit_NeoPixel : public NPB_IMPL {
    public:
        Adafruit_NeoPixel(const uint16_t numberOfLeds, const uint8_t pinNumber = 2, const neoPixelType type = 0)
        : NPB_IMPL(numberOfLeds, pinNumber),
          numLEDs(numberOfLeds) {
        }
        
        void begin() {
            NPB_IMPL::Begin();
            NPB_IMPL::Show();
            numBytes = NPB_IMPL::PixelsSize();
        }
        
        void show() {
            NPB_IMPL::Show();
        }
        
        void delay(const uint32_t ms) const {
            vTaskDelay(ms / portTICK_PERIOD_MS);
        }
        
        void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
            NPB_IMPL::SetPixelColor(n, COLOR_FEATURE::ColorObject(r, g, b));
        }

        void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
            auto color = COLOR_FEATURE::ColorObject(r, g, b);
            NPB_IMPL::SetPixelColor(n, color);
        }

        void setPixelColor(uint16_t n, uint32_t c) {
            uint8_t r = (c >> 16) & 0xFF;
            uint8_t g = (c >>  8) & 0xFF;
            uint8_t b =  c        & 0xFF;
            auto color = COLOR_FEATURE::ColorObject(r, g, b);
            NPB_IMPL::SetPixelColor(n, color);
        }
        
        uint32_t getPixelColor(uint16_t n) const {
            auto color = NPB_IMPL::GetPixelColor(n);
            uint32_t c = 0;
            c |= color.R << 16;
            c |= color.G << 8;
            c |= color.B;
            return c;
        }
        
        void clear() {
            NPB_IMPL::ClearTo(COLOR_FEATURE::ColorObject(0, 0, 0));
        }
        
        void fill(uint32_t c, uint16_t first, uint16_t count) {
            uint8_t r = (c >> 16) & 0xFF;
            uint8_t g = (c >>  8) & 0xFF;
            uint8_t b =  c        & 0xFF;
            auto color = COLOR_FEATURE::ColorObject(r, g, b);
            NPB_IMPL::ClearTo(color, first, first+count-1);
        }

        uint16_t numPixels(void) const {
            return numLEDs;
        }
        
        void setBrightness(uint8_t b) {
            brightness = b;
            NPB_IMPL::SetBrightness(b);
        }
        
        uint8_t getBrightness() {
            brightness = NPB_IMPL::GetBrightness();
            return brightness;
        }
        
        uint8_t* getPixels() {
            return NPB_IMPL::Pixels();
        }
        
        void updateLength(uint16_t n) {
        }
        
        template<typename T>        
        T sine8(const T& x) const {
            return _sineTable[x]; // 0-255 in, 0-255 out
        }
        
        template<typename T>
        T gamma8(const T& x) const {
            return _gammaTable[x]; // 0-255 in, 0-255 out        
        }
        
    protected:
        uint8_t *pixels;
        uint16_t numLEDs;
        uint16_t numBytes;
        uint8_t brightness;
        static constexpr uint8_t  rOffset = COLOR_FEATURE::PixelSize;
        static constexpr uint8_t  wOffset = COLOR_FEATURE::PixelSize;
        
    private:
        /* Similar to above, but for an 8-bit gamma-correction table.
           Copy & paste this snippet into a Python REPL to regenerate:
        import math
        gamma=2.6
        for x in range(256):
            print("{:3},".format(int(math.pow((x)/255.0,gamma)*255.0+0.5))),
            if x&15 == 15: print
        */
        static constexpr uint8_t _sineTable[] = {
            128,131,134,137,140,143,146,149,152,155,158,162,165,167,170,173,
            176,179,182,185,188,190,193,196,198,201,203,206,208,211,213,215,
            218,220,222,224,226,228,230,232,234,235,237,238,240,241,243,244,
            245,246,248,249,250,250,251,252,253,253,254,254,254,255,255,255,
            255,255,255,255,254,254,254,253,253,252,251,250,250,249,248,246,
            245,244,243,241,240,238,237,235,234,232,230,228,226,224,222,220,
            218,215,213,211,208,206,203,201,198,196,193,190,188,185,182,179,
            176,173,170,167,165,162,158,155,152,149,146,143,140,137,134,131,
            128,124,121,118,115,112,109,106,103,100, 97, 93, 90, 88, 85, 82,
             79, 76, 73, 70, 67, 65, 62, 59, 57, 54, 52, 49, 47, 44, 42, 40,
             37, 35, 33, 31, 29, 27, 25, 23, 21, 20, 18, 17, 15, 14, 12, 11,
             10,  9,  7,  6,  5,  5,  4,  3,  2,  2,  1,  1,  1,  0,  0,  0,
              0,  0,  0,  0,  1,  1,  1,  2,  2,  3,  4,  5,  5,  6,  7,  9,
             10, 11, 12, 14, 15, 17, 18, 20, 21, 23, 25, 27, 29, 31, 33, 35,
             37, 40, 42, 44, 47, 49, 52, 54, 57, 59, 62, 65, 67, 70, 73, 76,
             79, 82, 85, 88, 90, 93, 97,100,103,106,109,112,115,118,121,124
        };
        
        /* Similar to above, but for an 8-bit gamma-correction table.
           Copy & paste this snippet into a Python REPL to regenerate:
        import math
        gamma=2.6
        for x in range(256):
            print("{:3},".format(int(math.pow((x)/255.0,gamma)*255.0+0.5))),
            if x&15 == 15: print
        */
        static constexpr uint8_t _gammaTable[] = {
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,
            1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,
            3,  3,  4,  4,  4,  4,  5,  5,  5,  5,  5,  6,  6,  6,  6,  7,
            7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 11, 12, 12,
           13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20,
           20, 21, 21, 22, 22, 23, 24, 24, 25, 25, 26, 27, 27, 28, 29, 29,
           30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 38, 38, 39, 40, 41, 42,
           42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
           58, 59, 60, 61, 62, 63, 64, 65, 66, 68, 69, 70, 71, 72, 73, 75,
           76, 77, 78, 80, 81, 82, 84, 85, 86, 88, 89, 90, 92, 93, 94, 96,
           97, 99,100,102,103,105,106,108,109,111,112,114,115,117,119,120,
          122,124,125,127,129,130,132,134,136,137,139,141,143,145,146,148,
          150,152,154,156,158,160,162,164,166,168,170,172,174,176,178,180,
          182,184,186,188,191,193,195,197,199,202,204,206,209,211,213,215,
          218,220,223,225,227,230,232,235,237,240,242,245,247,250,252,255
        };
};