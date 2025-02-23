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
    Freeimage,
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
            if (wcsstr(L",png,", ext))  return Png;
            if (wcsstr(L",bmp,dib,", ext))  return Bmp;
            if (wcsstr(L",gif,", ext))  return Gif;
            if (wcsstr(L",tiff,tif,", ext))  return Tiff;
            if (wcsstr(L",ico,icon,", ext))  return Icon;
            if (wcsstr(L",psd,", ext))  return Psd;
            if (wcsstr(L",tga,", ext))  return Tga;
            if (wcsstr(L",webp,", ext))  return Webp;
            if (wcsstr(ExtRaw, ext))  return Raw;
            if (wcsstr(L",dds,", ext))  return Dds;
            if (wcsstr(L",dng,", ext))  return Dng;
            if (wcsstr(L",svg,", ext))  return Svg;
            if (wcsstr(L",heif,heic,", ext))  return Heif;
            if (wcsstr(L",avif,", ext))  return Avif;
            if (wcsstr(ExtFreeimage, ext))  return Freeimage;
        }
        return Unknown;
    }

public:
    static constexpr PCWSTR   ExtJpeg = L",jpg,jpeg,jfif,jpe,";
    static constexpr PCWSTR   ExtRaw = L",3fr,ari,arw,bay,cap,cr2,cr3,crw,dcs,dcr,drf,eip,erf,fff,iiq,k25,kdc,mef,mos,mrw,nef,nrw,orf,ori,pef,ptx,pxn,raf,raw,rw2,rwl,sr2,srf,srw,x3f,dng,";
    static constexpr PCWSTR   ExtFreeimage = L",exr,g3,hdr,iff,lbm,j2k,j2c,jp2,jxr,wdp,hdp,pcd,pcx,pfm,pct,pict,pic,pbm,pgm,ppm,ras,sgi,rgb,rgba,bw,wap,wbmp,wbm,xbm,xpm,";
};

_PHOXO_NAMESPACE_END
