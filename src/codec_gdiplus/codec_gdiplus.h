#pragma once
#include "gdiplus_encode_param.h"

_PHOXO_BEGIN

/// Read / Write image using Gdi+.
class CodecGdiplus
{
public:
    static Image LoadFile(PCWSTR filepath, Gdiplus::PixelFormat output_format = PixelFormat32bppARGB)
    {
        auto   sp = WIC::CreateStreamFromFileNoLock(filepath);
        return LoadStream(sp, output_format);
    }

    static Image LoadStream(IStream* sp, Gdiplus::PixelFormat output_format)
    {
        Gdiplus::Bitmap   src(sp);
        return ImageHandler::Make(src, output_format);
    }

    static bool SaveFile(PCWSTR filepath, const Image& img, int jpeg_quality = 0, int dpi = 0)
    {
        auto   src = CreateBitmapReference(img);
        if (!src)
            return false;

        if (dpi)
        {
            src->SetResolution((float)dpi, (float)dpi);
        }

        internal::GdiplusSaveParam   param(filepath, jpeg_quality);
        return (src->Save(filepath, &param.m_type_CLSID, param.m_encoder_param.get()) == Gdiplus::Ok);
    }

    static std::unique_ptr<Gdiplus::Bitmap> CreateBitmapReference(const Image& img)
    {
        if (!img) { assert(false); return nullptr; }
        return std::make_unique<Gdiplus::Bitmap>(img.Width(), img.Height(), img.GetStride(), GetPixelFormat(img), img.GetMemStart());
    }

private:
    static Gdiplus::PixelFormat GetPixelFormat(const Image& img)
    {
        switch (img.ColorBits())
        {
            case 8: return PixelFormat8bppIndexed;
            case 24: return PixelFormat24bppRGB;
            case 32: return (img.IsPremultiplied() ? PixelFormat32bppPARGB : PixelFormat32bppARGB);
        }
        return PixelFormatUndefined;
    }
};

_PHOXO_NAMESPACE_END
