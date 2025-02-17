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
        CString   ext = PathFindExtension(filepath);
        if (ext.GetLength())
            ext.Delete(0, 1);
        ext = L"," + ext + L",";

        using enum ImageFormat;
        if (StrStrI(ImageExtJpeg(), ext))  return Jpeg;
        if (StrStrI(L",png,", ext))  return Png;
        if (StrStrI(L",bmp,dib,", ext))  return Bmp;
        if (StrStrI(L",gif,", ext))  return Gif;
        if (StrStrI(L",tiff,tif,", ext))  return Tiff;
        if (StrStrI(L",ico,icon,", ext))  return Icon;
        if (StrStrI(L",psd,", ext))  return Psd;
        if (StrStrI(L",tga,", ext))  return Tga;
        if (StrStrI(L",webp,", ext))  return Webp;
        if (StrStrI(ImageExtRaw(), ext))  return Raw;
        if (StrStrI(L",dds,", ext))  return Dds;
        if (StrStrI(L",dng,", ext))  return Dng;
        if (StrStrI(L",svg,", ext))  return Svg;
        if (StrStrI(L",heif,heic,", ext))  return Heif;
        if (StrStrI(L",avif,", ext))  return Avif;
        if (StrStrI(ImageExtFreeimage(), ext))  return Freeimage;
        return Unknown;
    }

public:
    static PCWSTR ImageExtJpeg() { return L",jpg,jpeg,jfif,jpe,"; }
    static PCWSTR ImageExtRaw() { return L",3fr,ari,arw,bay,cap,cr2,cr3,crw,dcs,dcr,drf,eip,erf,fff,iiq,k25,kdc,mef,mos,mrw,nef,nrw,orf,ori,pef,ptx,pxn,raf,raw,rw2,rwl,sr2,srf,srw,x3f,dng,"; }
    static PCWSTR ImageExtFreeimage() { return L",exr,g3,hdr,iff,lbm,j2k,j2c,jp2,jxr,wdp,hdp,pcd,pcx,pfm,pct,pict,pic,pbm,pgm,ppm,ras,sgi,rgb,rgba,bw,wap,wbmp,wbm,xbm,xpm,"; }
};

_PHOXO_NAMESPACE_END
