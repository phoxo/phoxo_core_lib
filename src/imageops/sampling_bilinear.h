#pragma once

_PHOXO_BEGIN

/// Helper class.
class SamplingBilinear
{
public:
    /**
     * Returns Zero if out of bounds.
     * For best performance, keep:
     *   -1.0f < pt.X < img.Width()
     *   -1.0f < pt.Y < img.Height()
     */
    static RGBA32bit Get(const Image& img, const GPointF& pt)
    {
        auto pixel = [&img](int ix, int iy) -> RGBA32bit {
            return img.IsInside(ix, iy) ? *(const RGBA32bit*)img.GetPixel(ix, iy) : RGBA32bit{}; // 以后需要加clamp
        };

        int   x = (int)floor(pt.X);
        int   y = (int)floor(pt.Y);

        return PixelFunc::CalcBilinear(pt.X - x, pt.Y - y,
            pixel(x, y),
            pixel(x + 1, y),
            pixel(x, y + 1),
            pixel(x + 1, y + 1));
    }
};

_PHOXO_NAMESPACE_END
