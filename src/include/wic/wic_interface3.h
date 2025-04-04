#pragma once

namespace WIC
{
    // 超过此尺寸，WIC 会返回有符号整数溢出错误
    inline constexpr int MAX_BITMAP_PIXELS = 0x7FFFFFFF / 4 - 100; // 100 is not necessary

    inline GUID GetContainerFormat(IWICBitmapDecoder* decoder)
    {
        GUID   t = {};
        if (decoder) { decoder->GetContainerFormat(&t); }
        return t;
    }

    inline UINT GetFrameCount(IWICBitmapDecoder* decoder)
    {
        UINT   t = 0;
        if (decoder) { decoder->GetFrameCount(&t); }
        return t;
    }

    inline IWICBitmapFrameDecodePtr GetFrame(IWICBitmapDecoder* decoder, UINT index)
    {
        IWICBitmapFrameDecodePtr   t;
        if (decoder) { decoder->GetFrame(index, &t); }
        return t;
    }

    inline CSize GetBitmapSize(IWICBitmapSource* src)
    {
        UINT   x = 0, y = 0;
        if (src) { src->GetSize(&x, &y); }
        return CSize(x, y);
    }

    inline int GetResolution(IWICBitmapSource* src)
    {
        double   x = 0, y = 0;
        if (src) { src->GetResolution(&x, &y); }
        return (int)(x + 0.5);
    }

    inline WICPixelFormatGUID GetPixelFormat(IWICBitmapSource* bmp)
    {
        WICPixelFormatGUID   t = {};
        if (bmp) { bmp->GetPixelFormat(&t); }
        return t;
    }

    inline IWICBitmapSourcePtr ConvertFormat(IWICBitmapSource* src, REFWICPixelFormatGUID dest_format)
    {
        if (src && (GetPixelFormat(src) != dest_format))
        {
            IWICBitmapSourcePtr   t;
            ::WICConvertBitmapSource(dest_format, src, &t); assert(t);
            return t;
        }
        return src;
    }

    inline UINT GetColorContextsCount(IWICBitmapFrameDecode* frame_decode)
    {
        UINT   t = 0;
        if (frame_decode) { frame_decode->GetColorContexts(0, NULL, &t); }
        return t;
    }

    inline int GetBitsPerPixel(WICPixelFormatGUID fmt)
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

    inline IStreamPtr CreateStreamFromFileNoLock(PCWSTR filepath)
    {
        IStreamPtr   t;
        SHCreateStreamOnFileEx(filepath, STGM_READ | STGM_SHARE_DENY_NONE, 0, FALSE, NULL, &t);
        return t;
    }
}
