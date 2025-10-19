#pragma once
#include "basic_pixel_iterator.h"
#include "basic_LUT.h"
#include "basic_orientation.h"
#include "hue_saturation.h"
#include "checker_fill.h"

_PHOXO_BEGIN
_PHOXO_EFFECT_BEGIN

/// Brightness and contrast (32 bit).
class BrightnessContrast : public ColorLUT
{
private:
    double   m_brightness;
    double   m_slant;
public:
    /// -100 <= brightness <= 100, 0 means not change\n -100 <= contrast <= 100, 0 means no change.
    BrightnessContrast(int brightness, int contrast)
    {
        m_brightness = (brightness * 0.01) / 2.0;
        m_slant = tan((contrast * 0.01 + 1) * 0.785398); // PI/4
    }
private:
    BYTE InitLUT(int idx) override
    {
        double   value = idx / 255.0;
        if (m_brightness < 0)
            value *= (1.0 + m_brightness);
        else
            value += ((1.0 - value) * m_brightness);

        value = (value - 0.5) * m_slant + 0.5;
        return Math::Clamp0255(value * 255);
    }
};

/// Invert color (32 bit).
class InvertColor : public ColorLUT
{
    BYTE InitLUT(int idx) override
    {
        return (BYTE)(255 - idx);
    }
};

_PHOXO_NAMESPACE_END
_PHOXO_NAMESPACE_END
