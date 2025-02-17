#pragma once

_PHOXO_BEGIN
_PHOXO_EFFECT_BEGIN

/// Resize image using WIC (32bpp).
class ResizeWic : public ImageEffect
{
private:
    CSize   m_new_size;

public:
    WICBitmapInterpolationMode   m_resize_mode = WICBitmapInterpolationModeHighQualityCubic;

public:
    ResizeWic(int width, int height) : m_new_size(width, height) {}
    ResizeWic(CSize new_size) : m_new_size(new_size) {}

private:
    ProcessMode QueryProcessMode() override
    {
        return ProcessMode::EntireMyself;
    }

    void ProcessEntire(Image& img, IProgressListener*) override
    {
        auto   format = (img.IsPremultiplied() ? WICPremultiplied32bpp : WICNormal32bpp);
        auto   old = CWICFunc::CreateBitmapFromHBITMAP(img, img.IsPremultiplied() ? WICBitmapUsePremultipliedAlpha : WICBitmapUseAlpha);
        img.Destroy();
        auto   scaled = CWICFunc::ScaleBitmap(old, m_new_size, m_resize_mode);
        CodecWIC::Load(scaled, img, format);
    }
};

_PHOXO_NAMESPACE_END
_PHOXO_NAMESPACE_END
