#pragma once

_PHOXO_BEGIN
_PHOXO_EFFECT_BEGIN

/// Generate an image with text shadow.
class TextShadow : public ImageEffect
{
public:
    struct Params
    {
        int   blur_radius = 5;
        SIZE   shadow_offset = { 3,3 };
        Color   shadow_color = Gdiplus::Color::Black;
        Color   text_color = Gdiplus::Color::White;
        int   shadow_opacity = 100; // 0 ~ 100, where 100 = fully opaque
    };

public:
    TextShadow(IDWriteTextLayout* layout, const Params& cfg)
        : m_mask(D2D::CreateTextAlphaMask(layout))
        , m_cfg(cfg)
    {
    }

private:
    Image   m_mask;
    const Params   m_cfg;

private:
    void CreateShadowedImage(Image& output, CPoint& shadow_pos, CPoint& original_pos) const
    {
        const CRect   rc(CPoint(), m_mask.Size());
        const CRect   shadow = rc + m_cfg.shadow_offset;
        CRect   glow = shadow;
        glow.InflateRect(m_cfg.blur_radius, m_cfg.blur_radius);
        CRect   total;
        total.UnionRect(rc, glow);

        output.Create(total.Size(), 32);
        output.ZeroPixels();
        shadow_pos = shadow.TopLeft() - total.TopLeft();
        original_pos = rc.TopLeft() - total.TopLeft();
    }

    void BlurImage(Image& img) const
    {
        StackBlur   fx(m_cfg.blur_radius);
        fx.EnableParallel(IsParallelEnabled());
        img.ApplyEffect(fx);
    }

    bool IsSupported(const Image& img) override
    {
        return m_mask.IsValid();
    }

    ProcessMode QueryProcessMode() override
    {
        return ProcessMode::EntireMyself;
    }

    void ProcessEntire(Image& img, IProgressListener*) override
    {
        CPoint   shadow, original;
        CreateShadowedImage(img, shadow, original);

        // back
        ImageHandler::Cover(img, m_mask, shadow);
        ScaleAlpha(img, m_cfg.shadow_opacity);

        ImageFastPixel::SetRGBKeepAlpha(img, m_cfg.shadow_color);
        BlurImage(img);

        // front
        ImageFastPixel::SetRGBKeepAlpha(m_mask, m_cfg.text_color);
        CompositeStraight   fx(m_mask, original);
        fx.EnableParallel(IsParallelEnabled());
        img.ApplyEffect(fx);
    }

    static void ScaleAlpha(Image& img, int percent)
    {
        ImageFastPixel::PixelSpan   s(img);
        if (!s || (percent >= 100))
            return;

        s.ForEachPixel([percent](auto& px) { px.a = (BYTE)((px.a * percent + 50) / 100); });
    }
};

_PHOXO_NAMESPACE_END
_PHOXO_NAMESPACE_END
