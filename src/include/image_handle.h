#pragma once

_PHOXO_BEGIN

/// Handles basic image operations.
class ImageHandler
{
public:
    static bool IsFullyOpaque(const Image& img)
    {
        if (img.ColorBits() != 32)
            return false;

        int   total = img.Width() * img.Height();
        auto   ptr = (const RGBA32bit*)img.GetMemStart();
        return std::all_of(ptr, ptr + total, [](auto& pixel) {
            return pixel.a == 0xFF;
        });
    }

    /**
        @name Draw Image.
        if it's a 32bpp image, it must be <span style='color:#FF0000'>premultipled</span>.
    */
    //@{
    /// draw image.
    static void Draw(HDC dc, POINT pt_on_dc, HBITMAP img)
    {
        CRect   rc(pt_on_dc, Utils::GetBitmapSize(img));
        Draw(dc, rc, img);
    }

    /// draw image.
    static void Draw(HDC dc, CRect dest, HBITMAP img, const RECT* rect_on_image = nullptr)
    {
        DIBSECTION   info = {};
        if (::GetObject(img, sizeof(info), &info))
        {
            CRect   rc = (rect_on_image ? *rect_on_image : CRect(0, 0, info.dsBm.bmWidth, info.dsBm.bmHeight));

            if ((info.dsBmih.biBitCount == 32) && info.dsBm.bmBits) // is a DIB bitmap
            {
                BLENDFUNCTION   bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
                ::GdiAlphaBlend(dc, dest.left, dest.top, dest.Width(), dest.Height(), BitmapHDC(img), rc.left, rc.top, rc.Width(), rc.Height(), bf);
            }
            else
            {
                int   old = SetStretchBltMode(dc, COLORONCOLOR);
                StretchBlt(dc, dest.left, dest.top, dest.Width(), dest.Height(), BitmapHDC(img), rc.left, rc.top, rc.Width(), rc.Height(), SRCCOPY);
                SetStretchBltMode(dc, old);
            }
        }
    }
    //@}

    /// @name Process Image.
    //@{
    /// get region of image, rect_on_image must inside image.
    static void GetRegion(const Image& img, CRect rect_on_image, Image& output)
    {
        CRect   rc;
        rc.IntersectRect(CRect(0, 0, img.Width(), img.Height()), rect_on_image);
        if (!rc.IsRectEmpty() &&
            (rc == rect_on_image) &&
            output.Create(rc.Width(), rc.Height(), img.ColorBits(), img.GetAttribute()))
        {
            int   copy_bytes = output.Width() * img.ColorBits() / 8;
            for (int y = 0; y < output.Height(); y++)
            {
                memcpy(output.GetLinePtr(y), img.GetPixel(rc.left, rc.top + y), copy_bytes);
            }
            return;
        }
        assert(false);
    }

    /// cover image.
    static void Cover(Image& bottom, const Image& top, POINT pt_on_bottom)
    {
        CRect   rect_top(pt_on_bottom, CSize(top.Width(), top.Height()));
        CRect   rc;
        rc.IntersectRect(CRect(0, 0, bottom.Width(), bottom.Height()), rect_top);
        if (!rc.IsRectEmpty() && (bottom.ColorBits() == top.ColorBits()) && (bottom.GetAttribute() == top.GetAttribute()))
        {
            for (int y = rc.top; y < rc.bottom; y++)
            {
                auto   dest = bottom.GetPixel(rc.left, y);
                auto   src = top.GetPixel(rc.left - pt_on_bottom.x, y - pt_on_bottom.y);
                memcpy(dest, src, rc.Width() * top.ColorBits() / 8);
            }
            return;
        }
        assert(false);
    }
    //@}
};

_PHOXO_NAMESPACE_END
