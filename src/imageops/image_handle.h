#pragma once

_PHOXO_BEGIN

/// Handles basic image operations.
class ImageHandler
{
public:
    /// @name Process Image.
    //@{
    /// get region of image, rect_on_image must inside image.
    static void GetRegion(const Image& img, CRect rect_on_image, Image& output)
    {
        CRect   rc;
        rc.IntersectRect(CRect(0, 0, img.Width(), img.Height()), rect_on_image);
        if (!rc.IsRectEmpty() &&
            (rc == rect_on_image) &&
            output.Create(rc.Size(), img.ColorBits(), img.Attribute()))
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
        if (!rc.IsRectEmpty() && (bottom.ColorBits() == top.ColorBits()) && (bottom.Attribute() == top.Attribute()))
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

    /// @name Create Image.
    //@{
    /// Creates an Image from a gdiplus bitmap.
    static Image Make(Gdiplus::Bitmap& src, Gdiplus::PixelFormat output_format)
    {
        int   attr = (output_format == PixelFormat32bppPARGB) ? Image::PremultipliedAlpha : 0;
        int   bpp = Gdiplus::GetPixelFormatSize(output_format);
        UINT   width = src.GetWidth(), height = src.GetHeight();

        Image   img;
        if (img.Create(width, height, bpp, attr))
        {
            Gdiplus::BitmapData   bd{ width, height, img.Stride(), output_format, img.GetMemStart() };
            Gdiplus::Rect   rgn(0, 0, width, height);
            auto   b = src.LockBits(&rgn, Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeUserInputBuf, output_format, &bd); assert(b == Gdiplus::Ok);
            src.UnlockBits(&bd);
        }
        return img;
    }

    /// Creates an Image from a WIC bitmap.
    static Image Make(IWICBitmapSource* src_bmp, WICPixelFormatGUID output_format)
    {
        int   attr = (output_format == WICPremultiplied32bpp) ? Image::PremultipliedAlpha : 0;
        int   bpp = GetBitsPerPixel(output_format);
        auto    src = WIC::ConvertFormat(src_bmp, output_format);
        CSize   sz = WIC::GetBitmapSize(src);

        Image   img;
        if (img.Create(sz, bpp, attr))
        {
            // 如果没装hevc ext, heif文件CopyPixels返回0xc00d5212 (Unsupported File Format)
            if (src->CopyPixels(NULL, img.Stride(), img.GetPixelBufferSize(), img.GetMemStart()) == S_OK)
                return img;
        }
        assert(false);
        img.Destroy();
        return img;
    }
    //@}

private:
    static int GetBitsPerPixel(WICPixelFormatGUID fmt)
    {
        // 目前只用到了 WICNormal32bpp / WICPremultiplied32bpp
        if ((fmt == WICNormal32bpp) ||
            (fmt == WICPremultiplied32bpp) ||
            (fmt == GUID_WICPixelFormat32bppBGR))
            return 32;
        if (fmt == GUID_WICPixelFormat24bppBGR)  return 24;
        assert(false);
        return 0;
    }
};

_PHOXO_NAMESPACE_END
