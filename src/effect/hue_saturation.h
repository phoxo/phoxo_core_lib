#pragma once

_PHOXO_BEGIN
_PHOXO_EFFECT_BEGIN

/// Hue and saturation (32 bit).
class HueSaturation : public PixelIterator<HueSaturation>
{
private:
    double   m_saturation;

public:
    /// -100 <= saturation <= 100, 0 means not change
    HueSaturation(int hue, int saturation)
    {
        m_saturation = saturation * 0.01;
    }

    void Process(RGBA32bit* px) const
    {
        double   h, s, l;
        RGBtoHSL(px, h, s, l);
        s = s * (1 + m_saturation);
        HSLtoRGB(h, s, l, px);
    }

    static void HandlePixel(Image&, int, int, RGBA32bit* px, HueSaturation& eff)
    {
        eff.Process(px);
    }

    static void HSLtoRGB(double h, double s, double l, RGBA32bit* out)
    {
        if (s == 0)
        {
            out->r = out->g = out->b = Math::Clamp0255(l * 255);
        }
        else
        {
            double   q = (l < 0.5 ? l * (1 + s) : l + s - l * s);
            double   p = 2 * l - q;
            out->r = Math::Clamp0255(hue2rgb(p, q, h + 1.0 / 3) * 255);
            out->g = Math::Clamp0255(hue2rgb(p, q, h) * 255);
            out->b = Math::Clamp0255(hue2rgb(p, q, h - 1.0 / 3) * 255);
        }
    }

    static void RGBtoHSL(const RGBA32bit* px, double& H, double& S, double& L)
    {
        double   r = px->r / 255.0;
        double   g = px->g / 255.0;
        double   b = px->b / 255.0;
        double   low = (std::min)(r, (std::min)(g, b));
        double   high = (std::max)(r, (std::max)(g, b));
        double   sum = high + low;

        L = sum / 2.0;

        if (high == low)
        {
            H = S = 0;
        }
        else
        {
            double   diff = high - low;
            if (L < 0.5)
                S = diff / sum;
            else
                S = diff / (2.0 - sum);

            if (r == high)
                H = (g - b) / diff;
            else if (g == high)
                H = 2 + (b - r) / diff;
            else
                H = 4 + (r - g) / diff;

            H /= 6.0;

            if (H < 0.0)
                H += 1.0;
            else if (H > 1.0)
                H -= 1.0;
        }
    }

private:
    static double hue2rgb(double p, double q, double t)
    {
        if (t < 0) t += 1;
        if (t > 1) t -= 1;
        if (t < 1.0 / 6) return p + (q - p) * 6 * t;
        if (t < 1.0 / 2) return q;
        if (t < 2.0 / 3) return p + (q - p) * (2.0 / 3 - t) * 6;
        return p;
    }
};

_PHOXO_NAMESPACE_END
_PHOXO_NAMESPACE_END
