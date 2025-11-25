#pragma once

_PHOXO_BEGIN

/// Fast per-pixel utilities for 32-bit images
class ImageFastPixel
{
public:
    /// parallel if pixels exceed
    static inline int   parallel_threshold = 500'000;

    /// Returns true if all pixels have alpha == 0xFF
    static bool IsFullyOpaque(const Image& img)
    {
        PixelSpan   s(img);
        if (!s)
            return false;

        auto   func = [](const auto& px) { return px.a == 0xFF; };

        if (s.use_parallel)
            return std::all_of(std::execution::par, s.begin, s.end, func);
        else
            return std::all_of(s.begin, s.end, func);
    }

    /// Sets RGB of all pixels to the specified color, keeping alpha unchanged
    static void SetRGBKeepAlpha(Image& img, Color clr)
    {
        PixelSpan   s(img);
        if (!s)
            return;

        s.ForEachPixel([clr](auto& px) { PixelFunc::CopyRGB(&px, &clr); });
        img.SetPremultiplied(false);
    }

    /// Pre-multiplies RGB by alpha
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
        Color   * begin{};
        Color   * end{};
        bool   use_parallel = false;

        PixelSpan(const Image& img)
        {
            if (img.ColorBits() == 32)
            {
                begin = (Color*)img.GetMemStart();
                end = begin + img.PixelCount();
                use_parallel = img.PixelCount() > parallel_threshold; // 0.5M pixels
            }
            else { assert(false); }
        }

        explicit operator bool() const { return begin != nullptr; }

        template<typename T>
        void ForEachPixel(T&& func) const
        {
            if (use_parallel)
                std::for_each(std::execution::par, begin, end, func);
            else
                std::for_each(begin, end, func);
        }
    };
};

_PHOXO_NAMESPACE_END
