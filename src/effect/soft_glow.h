#pragma once

_PHOXO_BEGIN
_PHOXO_EFFECT_BEGIN

/// Soft glow (32 bit).
class SoftGlow : public PixelIterator<SoftGlow>
{
private:
    Image   m_bak;
    BrightnessContrast   m_bc;
    StackBlur   m_gauss_blur;

public:
    /// radius >= 1 \n -100 <= brightness <= 100 \n -100 <= contrast <= 100.
    SoftGlow(int radius, int brightness, int contrast) : m_gauss_blur(radius), m_bc(brightness, contrast)
    {
    }

    static void HandlePixel(Image& img, int x, int y, RGBA32bit* px, SoftGlow& effect)
    {
        effect.Process(x, y, *px);
    }

private:
    void OnBeforeProcess(Image& img) override
    {
        m_bak = img;

        m_gauss_blur.EnableParallel(IsParallelEnabled());
        img.ApplyEffect(m_gauss_blur);

        m_bc.EnableParallel(IsParallelEnabled());
        img.ApplyEffect(m_bc);
    }

    void ProcessChannel(BYTE& dst, const BYTE& origin) const
    {
        dst = Math::Clamp0255(255 - (255 - origin) * (255 - dst) / 255);
    }

    void Process(int x, int y, RGBA32bit& px) const
    {
        auto   origin = (RGBA32bit*)m_bak.GetPixel(x, y);
        ProcessChannel(px.r, origin->r);
        ProcessChannel(px.g, origin->g);
        ProcessChannel(px.b, origin->b);
        px.a = origin->a;
    }
};

_PHOXO_NAMESPACE_END
_PHOXO_NAMESPACE_END
