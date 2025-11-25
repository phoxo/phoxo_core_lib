#pragma once

_PHOXO_BEGIN

/// Sampling helper: computes the weighted average color of a subpixel area.
class SamplingAreaBox
{
public:
    static Color Get(const Image& img, GPointF pt, double ratio)
    {
        PixelAreaBox   box(pt, ratio);

        // If completely outside the image, return
        if (box.right <= 0 || box.bottom <= 0 || box.left >= img.Width() || box.top >= img.Height())
            return {};

        CRect   loop = box.GetLoopRect();
        for (int y = loop.top; y <= loop.bottom; y++) // Inclusive range: must use <=
        {
            double   h = box.OverlapY(y);
            if (h <= 0) continue;

            for (int x = loop.left; x <= loop.right; x++) // must use <=
            {
                double   w = box.OverlapX(x);
                if (w <= 0) continue;

                if (img.IsInside(x, y))
                {
                    box.Accumulate(*(const Color*)img.GetPixel(x, y), w * h);
                }
            }
        }
        return box.ResultColor();
    }

private:
    // Overlapping length of [i, i+1) and [low, high), used as weight in calculation.
    static double OverlapLength(double i, double low, double high)
    {
        return (std::min)(i + 1, high) - (std::max)(i, low);
    }

    struct PixelAreaBox
    {
        double   left, top, right, bottom;
        double   sb = 0, sg = 0, sr = 0, sa = 0, total_weight = 0;

        PixelAreaBox(GPointF pt, double ratio)
        {
            double   radius = 0.5 / ratio;
            left = pt.X - radius; top = pt.Y - radius;
            right = pt.X + radius; bottom = pt.Y + radius;
        }

        // Integer bounding box (inclusive): x <= right
        CRect GetLoopRect() const
        {
            return CRect((int)floor(left), (int)floor(top), (int)floor(right), (int)floor(bottom));
        }

        double OverlapX(int x) const { return OverlapLength(x, left, right); }
        double OverlapY(int y) const { return OverlapLength(y, top, bottom); }

        void Accumulate(const Color& px, double weight)
        {
            px.PremulSum(sb, sg, sr, sa, weight);
            total_weight += weight;
        }

        Color ResultColor() const
        {
            Color   ret{};
            if (total_weight > 0) // total_weight > 0 if accumulated, 0 if outside the layer
            {
                ret.a = (BYTE)(sa / total_weight + 0.5);
                if (ret.a) // May be fully transparent
                {
                    ret.b = (BYTE)(sb / sa + 0.5);
                    ret.g = (BYTE)(sg / sa + 0.5);
                    ret.r = (BYTE)(sr / sa + 0.5);
                }
            }
            return ret;
        }
    };
};

_PHOXO_NAMESPACE_END
