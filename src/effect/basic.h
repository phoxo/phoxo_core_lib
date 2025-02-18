#pragma once
#include "basic_pixel_iterator.h"
#include "basic_LUT.h"
#include "basic_orientation.h"
#include "hue_saturation.h"

_PHOXO_BEGIN
_PHOXO_EFFECT_BEGIN

/// Gray scale (32 bit).
class Grayscale : public PixelIterator<Grayscale>
{
public:
    static void HandlePixel(Image& img, int x, int y, RGBA32bit* px, ImageEffect& effect)
    {
        px->r = px->g = px->b = Color::GetGrayscale(px);
    }
};

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

/// Pre-multiply image (32 bit).
class Premultiply : public PixelIterator<Premultiply>
{
private:
    bool IsSupported(const Image& img) override
    {
        return (img.ColorBits() == 32) && !img.IsPremultiplied();
    }

    void OnAfterProcess(Image& img) override
    {
        img.SetPremultiplied(true);
    }

public:
    static void HandlePixel(Image& img, int x, int y, RGBA32bit* px, ImageEffect& effect)
    {
        Color::Premultiply(*px);
    }
};

/// Un-Premultiply image (32 bit).
class UnPremultiply : public PixelIterator<UnPremultiply>
{
private:
    bool IsSupported(const Image& img) override
    {
        return (img.ColorBits() == 32) && img.IsPremultiplied();
    }

    void OnAfterProcess(Image& img) override
    {
        img.SetPremultiplied(false);
    }

public:
    static void HandlePixel(Image& img, int x, int y, RGBA32bit* px, ImageEffect& effect)
    {
        Color::UnPremultiply(*px);
    }
};

_PHOXO_NAMESPACE_END
_PHOXO_NAMESPACE_END
