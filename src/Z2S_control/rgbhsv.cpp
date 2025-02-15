#include "rgbhsv.h"

// all normalized
ColorHSV rgbToHsv(ColorRGB rgb)
{
    const auto& [r, g, b] = rgb;

    const auto chromaMax   = std::max(std::max(r, g), b);
    const auto chromaMin   = std::min(std::min(r, g), b);
    const auto chromaDelta = chromaMax - chromaMin;

    const auto saturation = chromaMax <= 0 ? 0 : chromaDelta / chromaMax;

    // clang-format off
    auto hue = [&] {
        if (chromaDelta <= 0) return 0.0f;   // undefined

        if      (chromaMax == r) return 60.0f * (std::fmod(((g - b) / chromaDelta), 6.0f));
        else if (chromaMax == g) return 60.0f * (((b - r) / chromaDelta) + 2);
        else if (chromaMax == b) return 60.0f * (((r - g) / chromaDelta) + 4);
        else    [[unlikely]]     return 0.0f;
    }();
    // clang-format on

    if (hue < 0) {
        hue = 360.0f + hue;
    }

    return { hue / 360.0f, saturation, chromaMax };
}

// all normalized
ColorRGB hsvToRgb(ColorHSV hsv)
{
    auto& [h, s, v]  = hsv;
    h               *= 360.0f;

    const auto chroma = v * s;
    const auto hPrime = (float)std::fmod(h / 60.0f, 6);
    const auto X      = chroma * float(1 - std::fabs(std::fmod(hPrime, 2) - 1));
    const auto m      = v - chroma;

    // clang-format off
    const auto [r, g, b] = [&]() -> ColorRGB {
        if      (hPrime <= 1) return { chroma, X, 0 };
        else if (hPrime <= 2) return { X, chroma, 0 };
        else if (hPrime <= 3) return { 0, chroma, X };
        else if (hPrime <= 4) return { 0, X, chroma };
        else if (hPrime <= 5) return { X, 0, chroma };
        else if (hPrime <= 6) return { chroma, 0, X };
        else    [[unlikely]]  return { 0, 0, 0 };
    }();
    // clang-format on

    return { r + m, g + m, b + m };
}