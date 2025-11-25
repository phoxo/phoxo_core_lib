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
    static Color Get(const Image& img, GPointF pt)
    {
        auto pixel = [&img](int ix, int iy) -> Color {
            return img.IsInside(ix, iy) ? *(const Color*)img.GetPixel(ix, iy) : Color{}; // 以后需要加clamp
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
