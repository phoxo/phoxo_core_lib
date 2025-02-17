#pragma once
#include "wic_interface.h"

class CWICFunc
{
public:
    static inline IWICImagingFactoryPtr   g_factory;

public:
    static HRESULT CreateWICFactory()
    {
        return g_factory.CreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER);
    }

    static GUID GetContainerFormat(IWICBitmapDecoder* decoder)
    {
        GUID   t = {};
        if (decoder) { decoder->GetContainerFormat(&t); }
        return t;
    }

    static UINT GetFrameCount(IWICBitmapDecoder* decoder)
    {
        UINT   t = 0;
        if (decoder) { decoder->GetFrameCount(&t); }
        return t;
    }

    static IWICBitmapFrameDecodePtr GetFrame(IWICBitmapDecoder* decoder, UINT index)
    {
        IWICBitmapFrameDecodePtr   t;
        if (decoder) { decoder->GetFrame(index, &t); }
        return t;
    }

    static IWICColorContextPtr CreateColorContext()
    {
        IWICColorContextPtr   t;
        g_factory->CreateColorContext(&t);
        return t;
    }

    static IWICColorContextPtr CreateSystemColorContext_SRGB()
    {
        auto   t = CreateColorContext();
        if (t) { t->InitializeFromExifColorSpace(1); }
        return t;
    }

    static UINT GetColorContextsCount(IWICBitmapFrameDecode* frame_decode)
    {
        UINT   t = 0;
        if (frame_decode) { frame_decode->GetColorContexts(0, NULL, &t); }
        return t;
    }

    static IWICColorContextPtr GetFirstColorContext(IWICBitmapFrameDecode* frame_decode)
    {
        UINT   actual_count = GetColorContextsCount(frame_decode);
        if (actual_count)
        {
            IWICColorContextPtr   icc = CreateColorContext();
            if (frame_decode->GetColorContexts(1, &icc.GetInterfacePtr(), &actual_count) == S_OK)
                return icc;
        }
        return NULL;
    }

    static CSize GetBitmapSize(IWICBitmapSource* src)
    {
        UINT   x = 0, y = 0;
        if (src) { src->GetSize(&x, &y); }
        return CSize(x, y);
    }

    static int GetResolution(IWICBitmapSource* src)
    {
        double   x = 0, y = 0;
        if (src) { src->GetResolution(&x, &y); }
        return (int)(x + 0.5);
    }

    static IWICBitmapSourcePtr ConvertFormat(IWICBitmapSourcePtr src, REFWICPixelFormatGUID dest_format)
    {
        IWICBitmapSourcePtr   t;
        if (src) { WICConvertBitmapSource(dest_format, src, &t); }
        return (t ? t : src);
    }

    static WICPixelFormatGUID GetPixelFormat(IWICBitmapSource* bmp)
    {
        WICPixelFormatGUID   t = {};
        if (bmp) { bmp->GetPixelFormat(&t); }
        return t;
    }

    static IWICComponentInfoPtr CreateComponentInfo(REFCLSID component)
    {
        IWICComponentInfoPtr   t;
        g_factory->CreateComponentInfo(component, &t);
        return t;
    }

    static UINT GetBitsPerPixel(REFWICPixelFormatGUID fmt)
    {
        IWICPixelFormatInfoPtr   info = CreateComponentInfo(fmt);
        UINT   bpp = 0;
        if (info) { info->GetBitsPerPixel(&bpp); }
        return bpp;
    }

    static IWICBitmapSourcePtr ScaleBitmap(IWICBitmapSourcePtr src, CSize dest_size, WICBitmapInterpolationMode mode = WICBitmapInterpolationModeHighQualityCubic)
    {
        // sometimes color error when scaling in PBGRA format (?´ý¿¼Ö¤)
        // assert(GetPixelFormat(src) == GUID_WICPixelFormat32bppBGRA);
        if (GetBitmapSize(src) == dest_size)
            return src;

        IWICBitmapScalerPtr   cmd;
        g_factory->CreateBitmapScaler(&cmd);
        if (cmd && src && (cmd->Initialize(src, dest_size.cx, dest_size.cy, mode) == S_OK))
            return cmd;
        return src;
    }

    static IWICBitmapPtr CreateBitmap(CSize image_size, REFWICPixelFormatGUID fmt)
    {
        IWICBitmapPtr   t;
        g_factory->CreateBitmap(image_size.cx, image_size.cy, fmt, WICBitmapCacheOnLoad, &t);
        return t;
    }

    static IWICBitmapPtr CreateBitmapFromSource(IWICBitmapSource* src)
    {
        IWICBitmapPtr   t;
        if (src) { g_factory->CreateBitmapFromSource(src, WICBitmapCacheOnLoad, &t); }
        return t;
    }

    static IWICBitmapPtr CreateBitmapFromHICON(HICON ico)
    {
        IWICBitmapPtr   t;
        if (ico) { g_factory->CreateBitmapFromHICON(ico, &t); }
        return t;
    }

    static IWICBitmapPtr CreateBitmapFromHBITMAP(HBITMAP src, WICBitmapAlphaChannelOption options)
    {
        IWICBitmapPtr   t;
        if (src) { g_factory->CreateBitmapFromHBITMAP(src, NULL, options, &t); }
        return t;
    }

    static IWICBitmapFlipRotatorPtr CreateBitmapFlipRotator()
    {
        IWICBitmapFlipRotatorPtr   t;
        g_factory->CreateBitmapFlipRotator(&t);
        return t;
    }

    // an extremely long-running operation caused by the FlipRotator. Caching the source in a bitmap before using FlipRotator will work around the issue.
    static IWICBitmapPtr FlipRotateBitmapSafe(IWICBitmapPtr src, WICBitmapTransformOptions flag)
    {
        if ((flag != WICBitmapTransformRotate0) && src)
        {
            auto   cmd = CreateBitmapFlipRotator();
            if (cmd && (cmd->Initialize(src, flag) == S_OK))
            {
                return CreateBitmapFromSource(cmd);
            }
        }
        assert(false);
        return src;
    }

    static IWICColorTransformPtr CreateColorTransformer()
    {
        IWICColorTransformPtr   t;
        g_factory->CreateColorTransformer(&t);
        return t;
    }

    static IWICBitmapDecoderPtr CreateDecoderFromFilename(PCWSTR image_path, DWORD desired_access = GENERIC_READ)
    {
        IWICBitmapDecoderPtr   t;
        g_factory->CreateDecoderFromFilename(image_path, NULL, desired_access, WICDecodeMetadataCacheOnDemand, &t);
        return t;
    }

    static IWICBitmapDecoderPtr CreateDecoderFromStream(IStream* stm)
    {
        IWICBitmapDecoderPtr   t;
        if (stm) { g_factory->CreateDecoderFromStream(stm, NULL, WICDecodeMetadataCacheOnDemand, &t); }
        return t;
    }

    static IWICBitmapDecoderPtr CreateDecoderFromFileNoLock(PCWSTR image_path)
    {
        auto   t = CreateStreamFromFileNoLock(image_path);
        return CreateDecoderFromStream(t);
    }

    static IStreamPtr CreateStreamFromFileNoLock(PCWSTR image_path)
    {
        IStreamPtr   t;
        SHCreateStreamOnFileEx(image_path, STGM_FAILIFTHERE | STGM_READ | STGM_SHARE_DENY_NONE, FILE_ATTRIBUTE_NORMAL, FALSE, NULL, &t);
        return t;
    }

    // NO embedded ICC and JPEG orientation applied
    static IWICBitmapPtr LoadPlainImage(PCWSTR filepath, REFWICPixelFormatGUID output_format)
    {
        auto   s = CreateStreamFromFileNoLock(filepath);
        return LoadPlainImage(s, output_format);
    }

    static IWICBitmapPtr LoadPlainImage(IStream* sp, REFWICPixelFormatGUID output_format)
    {
        auto   decoder = CreateDecoderFromStream(sp);
        auto   frame = CWICFunc::GetFrame(decoder, 0); // first frame
        auto   dest = CWICFunc::ConvertFormat(frame, output_format); assert(dest);
        return CreateBitmapFromSource(dest);
    }
};
