#pragma once

_PHOXO_BEGIN

/// Helper.
class PixelFunc
{
public:
    /// Bilinear interpolation at (x, y) using 4 pixels: p00 [0,0], p10 [1,0], p01 [0,1], p11 [1,1]
    /// x, y ¡Ê [0, 1), relative to p00
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
};

_PHOXO_NAMESPACE_END
