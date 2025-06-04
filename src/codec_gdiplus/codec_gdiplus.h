#pragma once
#include "gdiplus_helper.h"
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
        auto   src = GdiplusHelper::CreateBitmapReference(img);
        if (!src)
            return false;

        if (dpi)
        {
            src->SetResolution((float)dpi, (float)dpi);
        }

        internal::GdiplusSaveParam   param(filepath, jpeg_quality);
        return (src->Save(filepath, &param.m_type_CLSID, param.m_encoder_param.get()) == Gdiplus::Ok);
    }
};

_PHOXO_NAMESPACE_END
