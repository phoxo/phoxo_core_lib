#pragma once
#include "basic_pixel_iterator.h"
#include "basic_LUT.h"
#include "basic_orientation.h"
#include "hue_saturation.h"
#include "checker_fill.h"

_PHOXO_BEGIN
_PHOXO_EFFECT_BEGIN

/// Gray scale (32 bit).
class Grayscale : public PixelIterator<Grayscale>
{
public:
    static void HandlePixel(Image&, int, int, RGBA32bit* px, ImageEffect&)
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
    static void HandlePixel(Image&, int, int, RGBA32bit* px, ImageEffect&)
    {
        PixelFunc::Premultiply(*px);
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
    static void HandlePixel(Image&, int, int, RGBA32bit* px, ImageEffect&)
    {
        PixelFunc::UnPremultiply(*px);
    }
};

/// Overlay color (32 bit).
class ColorOverlay : public PixelIterator<ColorOverlay>
{
private:
    Color   m_cr;
    bool   m_apply_premultiply;

public:
    // the alpha value of the cr will be ignored.
    ColorOverlay(Color cr, bool apply_premultiply = true) : m_cr(cr), m_apply_premultiply(apply_premultiply) {}

    static void HandlePixel(Image&, int, int, RGBA32bit* px, ColorOverlay& eff)
    {
        memcpy(px, &eff.m_cr, 3);
        if (eff.m_apply_premultiply)
        {
            PixelFunc::Premultiply(*px);
        }
    }

    void OnAfterProcess(Image& img) override
    {
        img.SetPremultiplied(m_apply_premultiply);
    }
};

/// Adjusts alpha channel by percentage (32 bit).  (Never tested it)
/*class AlphaPercent : public PixelIterator<AlphaPercent>
{
private:
    int   m_percent;

    bool IsSupported(const Image& img) override
    {
        return (img.ColorBits() == 32) && !img.IsPremultiplied();
    }

public:
    /// 0 <= percent <= 100
    AlphaPercent(int percent) : m_percent(percent) {}

    static void HandlePixel(Image&, int, int, RGBA32bit* px, AlphaPercent& eff)
    {
        px->a = (px->a * eff.m_percent + 50) / 100;
    }
};*/

_PHOXO_NAMESPACE_END
_PHOXO_NAMESPACE_END
