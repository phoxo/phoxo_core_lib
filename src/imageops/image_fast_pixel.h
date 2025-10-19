#pragma once

_PHOXO_BEGIN

/// Fast per-pixel utilities for 32-bit images.
class ImageFastPixel
{
public:
    /// Returns true if all pixels have alpha == 0xFF.
    static bool IsFullyOpaque(const Image& img)
    {
        PixelSpan   s(img);
        if (!s)
            return false;

        if (s.IsParallel())
            return std::all_of(std::execution::par, s.begin, s.end, [](auto& px) { return px.a == 0xFF; });
        else
            return std::all_of(s.begin, s.end, [](auto& px) { return px.a == 0xFF; });
    }

    /// Sets RGB of all pixels to the specified color, keeping alpha unchanged.
    static void SetRGBKeepAlpha(Image& img, RGBA32bit clr)
    {
        PixelSpan   s(img);
        if (!s)
            return;

        s.ForEachPixel([&](auto& px) { PixelFunc::CopyRGB(&px, &clr); });
        img.SetPremultiplied(false);
    }

    /// Pre-multiplies RGB by alpha.
    static void Premultiply(Image& img)
    {
        PixelSpan   s(img);
        if (!s || img.IsPremultiplied())
        {
            assert(false); return;
        }

        s.ForEachPixel([](auto& px) { PixelFunc::Premultiply(px); });
        img.SetPremultiplied(true);
    }

public:
    struct PixelSpan
    {
        RGBA32bit   * begin{};
        RGBA32bit   * end{};

        PixelSpan(const Image& img)
        {
            if (img.ColorBits() == 32)
            {
                begin = (RGBA32bit*)img.GetMemStart();
                end = begin + img.PixelCount();
            }
            else { assert(false); }
        }

        explicit operator bool() const { return begin != nullptr; }

        bool IsParallel() const
        {
            return (end - begin) > 500'000; // 0.5M pixels
        }

        template<typename T>
        void ForEachPixel(T&& func) const
        {
            if (IsParallel())
                std::for_each(std::execution::par, begin, end, func);
            else
                std::for_each(begin, end, func);
        }
    };
};

_PHOXO_NAMESPACE_END
