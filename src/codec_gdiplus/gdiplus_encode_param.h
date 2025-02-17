#pragma once

_PHOXO_BEGIN
_PHOXO_INTERNAL_BEGIN
/// @cond
struct GdiplusSaveParam
{
    CLSID   m_type_CLSID;
    ULONG   m_jpeg_quality;
    std::unique_ptr<Gdiplus::EncoderParameters>   m_encoder_param;

    GdiplusSaveParam(PCWSTR filepath, int jpeg_quality)
    {
        auto   image_type = ImageFileExtParser::GetType(filepath);
        m_type_CLSID = GetEncoderClsid(image_type);
        m_jpeg_quality = jpeg_quality;

        if ((image_type == ImageFormat::Jpeg) && jpeg_quality)
        {
            m_encoder_param.reset(new Gdiplus::EncoderParameters);
            m_encoder_param->Count = 1;
            m_encoder_param->Parameter[0] = { Gdiplus::EncoderQuality, 1, Gdiplus::EncoderParameterValueTypeLong, &m_jpeg_quality };
        }
    }

private:
    static GUID GetFormatGUID(ImageFormat fmt)
    {
        using enum ImageFormat;
        switch (fmt)
        {
            case Bmp: return Gdiplus::ImageFormatBMP;
            case Jpeg: return Gdiplus::ImageFormatJPEG;
            case Gif: return Gdiplus::ImageFormatGIF;
            case Tiff: return Gdiplus::ImageFormatTIFF;
            case Png: return Gdiplus::ImageFormatPNG;
        }
        return GUID_NULL;
    }

    static CLSID GetEncoderClsid(ImageFormat img_type)
    {
        UINT   num = 0, buf_size = 0;
        Gdiplus::GetImageEncodersSize(&num, &buf_size);
        if (num && buf_size)
        {
            std::vector<BYTE>   temp_buf(buf_size);
            auto   info = (Gdiplus::ImageCodecInfo*)temp_buf.data();
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
