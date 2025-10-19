#pragma once

_PHOXO_BEGIN

/// 32-bit RGBA color, inheriting from RGBQUAD.
class Color : public RGBQUAD
{
public:
    Color(int r, int g, int b, int a = 0xFF)
    {
        rgbRed = (BYTE)r;
        rgbGreen = (BYTE)g;
        rgbBlue = (BYTE)b;
        rgbReserved = (BYTE)a;
    }

    Color(const RGBQUAD& c)
    {
        *(RGBQUAD*)this = c;
    }

    Color(DWORD c = 0)
    {
        *(DWORD*)this = c;
    }

    static Color FromCOLORREF(COLORREF c)
    {
        return Color(GetRValue(c), GetGValue(c), GetBValue(c), 0xFF);
    }

    operator COLORREF() const
    {
        return RGB(rgbRed, rgbGreen, rgbBlue);
    }

    operator RGBA32bit&() const
    {
        return *(RGBA32bit*)this;
    }

    operator Gdiplus::Color() const
    {
        return Gdiplus::Color(rgbReserved, rgbRed, rgbGreen, rgbBlue);
    }

    bool IsColorLight() const
    {
        return (((5 * rgbGreen) + (2 * rgbRed) + rgbBlue) > (8 * 128));
    }

    /// px is 24 or 32 bpp format.
    static BYTE GetGrayscale(const RGBA32bit* px)
    {
        return (BYTE)((30 * px->r + 59 * px->g + 11 * px->b) / 100);
    }

    static constexpr RGBA32bit   Zero{};
};

_PHOXO_NAMESPACE_END
