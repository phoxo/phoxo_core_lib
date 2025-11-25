#pragma once

_PHOXO_BEGIN
_PHOXO_INTERNAL_BEGIN

/// @cond
struct GdiplusSaveParams
{
    CLSID   m_type_CLSID;
    ULONG   m_jpeg_quality;
    unique_ptr<Gdiplus::EncoderParameters>   m_encoder_param;

    GdiplusSaveParams(PCWSTR filepath, int jpeg_quality)
    {
        auto   image_type = ImageFileExtParser::GetType(filepath);
        m_type_CLSID = GetEncoderClsid(image_type);
        m_jpeg_quality = jpeg_quality;

        if ((image_type == ImageFormat::Jpeg) && jpeg_quality)
        {
            m_encoder_param = make_unique<Gdiplus::EncoderParameters>();
            m_encoder_param->Count = 1;
            m_encoder_param->Parameter[0] = { Gdiplus::EncoderQuality, 1, Gdiplus::EncoderParameterValueTypeLong, &m_jpeg_quality };
        }
    }

private:
    static GUID GetFormatGUID(ImageFormat fmt)
    {
        using enum ImageFormat;
        using namespace Gdiplus;
        switch (fmt)
        {
            case Bmp: return ImageFormatBMP;
            case Jpeg: return ImageFormatJPEG;
            case Gif: return ImageFormatGIF;
            case Tiff: return ImageFormatTIFF;
            case Png: return ImageFormatPNG;
        }
        return GUID_NULL;
    }

    static CLSID GetEncoderClsid(ImageFormat img_type)
    {
        UINT   num = 0, buf_size = 0;
        Gdiplus::GetImageEncodersSize(&num, &buf_size);
        if (num && buf_size)
        {
            std::vector<BYTE>   tempbuf(buf_size);
            auto   info = (Gdiplus::ImageCodecInfo*)tempbuf.data();
            Gdiplus::GetImageEncoders(num, buf_size, info);

            GUID   fmtid = GetFormatGUID(img_type);
            for (UINT i = 0; i < num; i++)
            {
                if (info[i].FormatID == fmtid)
                {
                    return info[i].Clsid;
                }
            }
        }
        return GUID_NULL;
    }
};
/// @endcond

_PHOXO_NAMESPACE_END
_PHOXO_NAMESPACE_END
