#pragma once

_PHOXO_BEGIN

// image format
enum class ImageFormat
{
    Unknown,
    Bmp,
    Jpeg,
    Gif,
    Tiff,
    Png,
    Icon,
    Psd,
    Tga,
    Webp,
    Raw,
    Dds,
    Dng,
    Svg,
    Heif,
    Avif,
    Jxl,
    StbLib,
    Exr,
    OpenJpeg,
};

/// Helper for parsing image file extensions.
class ImageFileExtParser
{
public:
    /// get image's format by file's ext name.
    static ImageFormat GetType(PCWSTR filepath)
    {
        using enum ImageFormat;

        CString   ext = PathFindExtension(filepath);
        if (ext.GetLength() >= 2)
        {
            ext.MakeLower();
            ext.SetAt(0, ',');
            ext.AppendChar(',');

            if (wcsstr(ExtJpeg, ext))  return Jpeg;
            if (ext == L",png,")  return Png;
            if (wcsstr(L",bmp,dib,", ext))  return Bmp;
            if (ext == L",gif,")  return Gif;
            if (wcsstr(L",tiff,tif,", ext))  return Tiff;
            if (wcsstr(L",ico,icon,", ext))  return Icon;
            if (ext == L",psd,")  return Psd;
            if (ext == L",tga,")  return Tga;
            if (ext == L",webp,")  return Webp;
            if (wcsstr(ExtRaw, ext))  return Raw;
            if (ext == L",dds,")  return Dds;
            if (ext == L",dng,")  return Dng;
            if (ext == L",svg,")  return Svg;
            if (wcsstr(L",heif,heic,", ext))  return Heif;
            if (ext == L",avif,")  return Avif;
            if (ext == L",jxl,")  return Jxl;
            if (wcsstr(L",hdr,ppm,pgm,", ext))  return StbLib;
            if (ext == L",exr,")  return Exr;
            if (wcsstr(L",j2k,jp2,", ext))  return OpenJpeg;
        }
        return Unknown;
    }

public:
    static constexpr PCWSTR   ExtJpeg = L",jpg,jpeg,jfif,jpe,";
    static constexpr PCWSTR   ExtRaw = L",arw,cr2,cr3,nef,orf,raf,rw2,dng,";
};

_PHOXO_NAMESPACE_END
