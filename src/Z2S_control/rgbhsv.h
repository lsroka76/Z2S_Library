#ifndef RGBHSV_H_
#define RGBHSV_H_

#include <cmath>


struct ColorRGB
{
    float m_r;
    float m_g;
    float m_b;
};

struct ColorHSV
{
    float m_h;
    float m_s;
    float m_v;
};

int32_t mapFloat(
    float x, float in_min, float in_max, float out_min, float out_max);

// all normalized
ColorHSV rgbToHsv(ColorRGB rgb);


// all normalized
ColorRGB hsvToRgb(ColorHSV hsv);

#endif