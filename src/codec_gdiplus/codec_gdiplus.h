#pragma once
#include "base_utils.h"
#include "text_render.h"
#include "codec_gdiplus_save_params.h"

_PHOXO_BEGIN

/// Read / Write image using Gdi+.
class CodecGdiplus
{
public:
    static Image LoadFile(PCWSTR filepath, Gdiplus::PixelFormat output_format = PixelFormat32bppARGB)
    {
        Gdiplus::Bitmap   src(filepath);
        return ImageHandler::Make(src, output_format);
    }

    static Image LoadStream(IStream* sp, Gdiplus::PixelFormat output_format)
    {
        Gdiplus::Bitmap   src(sp);
        return ImageHandler::Make(src, output_format);
    }

    static bool SaveFile(PCWSTR filepath, const Image& img, int jpeg_quality = 0, int dpi = 0)
    {
        auto   src = GdiplusUtils::CreateBitmapReference(img);
        if (!src)
            return false;

        if (dpi)
            src->SetResolution((float)dpi, (float)dpi);

        internal::GdiplusSaveParams   param(filepath, jpeg_quality);
        return src->Save(filepath, &param.m_type_CLSID, param.m_encoder_param.get()) == Gdiplus::Ok;
    }
};

_PHOXO_NAMESPACE_END
