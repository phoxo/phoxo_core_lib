#pragma once

_PHOXO_BEGIN
_PHOXO_EFFECT_BEGIN

/// Resize image using GDI+ (32 bit).
class ResizeGdiplus : public ImageEffect
{
private:
    const CSize   m_new_size;

public:
    Gdiplus::InterpolationMode   m_resize_mode = Gdiplus::InterpolationModeHighQualityBicubic;

public:
    ResizeGdiplus(int width, int height) : m_new_size(width, height) {}
    ResizeGdiplus(CSize new_size) : m_new_size(new_size) {}

private:
    ProcessMode QueryProcessMode() override
    {
        return ProcessMode::EntireMyself;
    }

    void ProcessEntire(Image& img, IProgressListener*) override
    {
        Image   old;
        img.Swap(old);
        auto   src = GdiplusUtils::CreateBitmapReference(old);

        img.Create(m_new_size, old.ColorBits(), old.Attribute());
        auto   dst = GdiplusUtils::CreateBitmapReference(img);
        if (src && dst)
        {
            Gdiplus::Graphics   gc(dst.get());
            DrawImage(gc, m_new_size, *src);
        }
    }

    void DrawImage(Gdiplus::Graphics& gc, CSize dst_size, Gdiplus::Bitmap& img) const
    {
        using namespace Gdiplus;

        ImageAttributes   attri;
        attri.SetWrapMode(WrapModeTileFlipXY);
        Rect   drc{ 0, 0, dst_size.cx, dst_size.cy };
        gc.SetPixelOffsetMode(PixelOffsetModeHalf);
        gc.SetInterpolationMode(m_resize_mode);
        gc.DrawImage(&img, drc, 0, 0, img.GetWidth(), img.GetHeight(), UnitPixel, &attri, NULL, NULL);
    }
};

_PHOXO_NAMESPACE_END
_PHOXO_NAMESPACE_END
