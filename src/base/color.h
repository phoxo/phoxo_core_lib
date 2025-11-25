#pragma once

_PHOXO_BEGIN

/// 32-bit BGRA color, compatible with RGBQUAD
union Color
{
public:
    RGBQUAD   quad;
    struct { BYTE b, g, r, a; };
    int32_t   val;

public:
    constexpr Color() : val(0)
    {
    }

    explicit constexpr Color(int red, int green, int blue, int alpha = 0xFF) : r((BYTE)red), g((BYTE)green), b((BYTE)blue), a((BYTE)alpha)
    {
    }

    explicit constexpr Color(RGBQUAD c) : quad(c)
    {
    }

    explicit constexpr Color(int32_t c) : val(c)
    {
    }

    operator RGBQUAD() const
    {
        return quad;
    }

    operator Gdiplus::Color() const
    {
        return Gdiplus::Color(a, r, g, b);
    }

    void operator=(int32_t c) { val = c; }
    void operator=(const RGBQUAD& c) { quad = c; }

    bool operator==(const Color& c) const { return val == c.val; }
    bool operator!=(const Color& c) const { return val != c.val; }

    bool IsColorLight() const
    {
        return (5 * g + 2 * r + b) > (8 * 128);
    }

    COLORREF ToCOLORREF() const
    {
        return RGB(r, g, b);
    }

    /// add premultiplied RGBA to accumulators
    void PremulSum(double& sb, double& sg, double& sr, double& sa, double coef) const
    {
        //  sb += (b * a) * coef;
        //  sg += (g * a) * coef;
        //  sr += (r * a) * coef;
        //  sa += a * coef;

        // the code below offers slightly better performance.
        double   ac = a * coef;
        sb += b * ac;
        sg += g * ac;
        sr += r * ac;
        sa += ac;
    }

    static Color FromCOLORREF(COLORREF c)
    {
        return Color(GetRValue(c), GetGValue(c), GetBValue(c));
    }

    /// px is 24 or 32 bpp format.
    static BYTE GetGrayscale(const Color* px)
    {
        return (BYTE)((30 * px->r + 59 * px->g + 11 * px->b) / 100);
    }
};

_PHOXO_NAMESPACE_END
