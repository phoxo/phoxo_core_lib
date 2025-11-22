#pragma once

namespace WIC
{
    // 超过此尺寸，WIC 会返回有符号整数溢出错误
    inline constexpr int  MAX_BITMAP_PIXELS = 0x7FFFFFFF / 4 - 100; // 100 is not necessary
    inline constexpr GUID  GUID_ContainerFormat_Jxl = { 0xfec14e3f, 0x427a, 0x4736, { 0xaa, 0xe6, 0x27, 0xed, 0x84, 0xf6, 0x93, 0x22 } };

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

    inline WICPixelFormatGUID GetPixelFormat(IWICBitmapSource* bmp)
    {
        WICPixelFormatGUID   t = {};
        if (bmp) { bmp->GetPixelFormat(&t); }
        return t;
    }

    inline IWICBitmapSourcePtr ConvertFormat(IWICBitmapSource* src, REFWICPixelFormatGUID dst_format)
    {
        if (src && (GetPixelFormat(src) != dst_format))
        {
            IWICBitmapSourcePtr   t;
            ::WICConvertBitmapSource(dst_format, src, &t); assert(t);
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

    inline IStreamPtr CreateStreamFromFileNoLock(PCWSTR filepath)
    {
        IStreamPtr   t;
        SHCreateStreamOnFileEx(filepath, STGM_READ | STGM_SHARE_DENY_NONE, 0, FALSE, NULL, &t);
        return t;
    }
}
