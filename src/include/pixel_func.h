#pragma once

_PHOXO_BEGIN

/// Helper.
class PixelFunc
{
public:
    /// Bilinear interpolation at (x, y) using 4 pixels: p00 [0,0], p10 [1,0], p01 [0,1], p11 [1,1]
    /// x, y �� [0, 1), relative to p00
    static RGBA32bit CalcBilinear(double x, double y, const RGBA32bit& p00, const RGBA32bit& p10, const RGBA32bit& p01, const RGBA32bit& p11)
    {
        // Calculate bilinear weights and accumulate contributions
        double   sb = 0, sg = 0, sr = 0, sa = 0;
        p00.PremulSum(sb, sg, sr, sa, (1.0 - x) * (1.0 - y));
        p10.PremulSum(sb, sg, sr, sa, x * (1.0 - y));
        p01.PremulSum(sb, sg, sr, sa, (1.0 - x) * y);
        p11.PremulSum(sb, sg, sr, sa, x * y);

        RGBA32bit   ret{ .a = (BYTE)(sa + 0.5) };
        if (ret.a)
        {
            ret.b = (BYTE)(sb / sa + 0.5);
            ret.g = (BYTE)(sg / sa + 0.5);
            ret.r = (BYTE)(sr / sa + 0.5);
        }
        return ret;
    }

    static void Premultiply(RGBA32bit& px)
    {
        if (px.a == 255) return;
        if (px.a == 0) { px = {}; return; }

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

    static void CompositeStraightAlpha(RGBA32bit& down, const RGBA32bit& up)
    {
        if (up.a == 0)
            return;
        if ((down.a == 0) || (up.a == 0xFF))
        {
            down = up;
            return;
        }

        /*
            OutA = up.a + down.a * (1 - up.a)
            r = (up.r*up.a + down.r*down.a * (1 - up.a)) / OutA
        */
        float   inv = 1.0f - up.a / 255.0f;
        float   outA = up.a + down.a * inv; // == 0 only if up.a && down.a == 0
        float   t0 = up.a / outA;
        float   t1 = down.a * inv / outA;

        down.b = (BYTE)(up.b * t0 + down.b * t1 + 0.5f); // Guaranteed in [0, 255.1]
        down.g = (BYTE)(up.g * t0 + down.g * t1 + 0.5f);
        down.r = (BYTE)(up.r * t0 + down.r * t1 + 0.5f);
        down.a = (BYTE)(outA + 0.5f);
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
};

_PHOXO_NAMESPACE_END
