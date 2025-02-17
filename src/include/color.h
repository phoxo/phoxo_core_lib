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
        return Gdiplus::Color(rgbRed, rgbGreen, rgbBlue);
    }

    static void Premultiply(RGBA32bit& px)
    {
        if (px.a == 255) return;
        if (px.a == 0) { (int32_t&)px = 0; return; }

        px.r = (BYTE)((px.r * px.a + 127) / 255); // faster : r * (a+1) / 256
        px.g = (BYTE)((px.g * px.a + 127) / 255);
        px.b = (BYTE)((px.b * px.a + 127) / 255);
    }

    static void UnPremultiply(RGBA32bit& px)
    {
        if (px.a == 255) return;
        if (px.a == 0) return;

        float   t = 255.0f / px.a; assert((px.r * t < 255.1) && (px.g * t < 255.1) && (px.b * t < 255.1));
        px.r = (BYTE)(std::min)(px.r * t + 0.5f, 255.0f);
        px.g = (BYTE)(std::min)(px.g * t + 0.5f, 255.0f);
        px.b = (BYTE)(std::min)(px.b * t + 0.5f, 255.0f);
    }

    static void CompositeStraightAlpha(RGBA32bit& dest, const RGBA32bit& up)
    {
        RGBA32bit   down = dest;
        if (up.a == 0)
            return;
        if ((down.a == 0) || (up.a == 0xFF))
        {
            dest = up;
            return;
        }

        /*
            OutA = up.a + down.a * (1 - up.a)
            r = (up.r*up.a + down.r*down.a * (1 - up.a)) / OutA
        */
        float   outA = up.a + down.a * (1.0f - up.a / 255.0f);
        float   t0 = up.a / outA;
        float   t1 = down.a * (1.0f - up.a / 255.0f) / outA;

        dest.b = (BYTE)(up.b * t0 + down.b * t1 + 0.5f);
        dest.g = (BYTE)(up.g * t0 + down.g * t1 + 0.5f);
        dest.r = (BYTE)(up.r * t0 + down.r * t1 + 0.5f);
        dest.a = (BYTE)(outA + 0.5f);
    }

    static void BlendStraightAlpha(RGBA32bit& down, const RGBA32bit& up)
    {
        if (up.a == 0)
            return;
        if (up.a == 0xFF)
        {
            memcpy(&down, &up, 3);
            return;
        }

        float   t = up.a / 255.0f;
        down.b = (BYTE)((up.b - down.b) * t + down.b + 0.5f);
        down.g = (BYTE)((up.g - down.g) * t + down.g + 0.5f);
        down.r = (BYTE)((up.r - down.r) * t + down.r + 0.5f);
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
};

_PHOXO_NAMESPACE_END
