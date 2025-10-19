#pragma once

_PHOXO_BEGIN

/// Static helpers for drawing HBITMAP images.
class ImageDrawer
{
public:
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
};

_PHOXO_NAMESPACE_END
