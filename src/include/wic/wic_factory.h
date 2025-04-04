#pragma once

// 这里的函数都依赖 g_factory
namespace WIC
{
    inline IWICImagingFactoryPtr   g_factory;

    inline void CreateWICFactory()
    {
        g_factory.CreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER);
    }

    inline IWICColorContextPtr CreateColorContext()
    {
        IWICColorContextPtr   t;
        g_factory->CreateColorContext(&t);
        return t;
    }

    inline IWICColorContextPtr CreateSystemColorContext_SRGB()
    {
        auto   t = CreateColorContext();
        if (t) { t->InitializeFromExifColorSpace(1); }
        return t;
    }

    inline IWICColorContextPtr GetFirstColorContext(IWICBitmapFrameDecode* frame)
    {
        if (UINT count = GetColorContextsCount(frame))
        {
            IWICColorContextPtr   icc = CreateColorContext();
            if (frame->GetColorContexts(1, &icc.GetInterfacePtr(), &count) == S_OK)
                return icc;
        }
        return nullptr;
    }

    inline IWICBitmapSourcePtr ScaleBitmap(IWICBitmapSource* src, CSize dest_size, WICBitmapInterpolationMode mode = WICBitmapInterpolationModeHighQualityCubic)
    {
        // sometimes color error when scaling in PBGRA format (?待考证)
        if (GetBitmapSize(src) == dest_size)
            return src;

        IWICBitmapScalerPtr   cmd;
        g_factory->CreateBitmapScaler(&cmd);
        if (cmd && src && (cmd->Initialize(src, dest_size.cx, dest_size.cy, mode) == S_OK))
            return cmd;
        assert(false);
        return src;
    }

    inline IWICBitmapPtr CreateBitmap(CSize image_size, REFWICPixelFormatGUID fmt)
    {
        IWICBitmapPtr   t;
        g_factory->CreateBitmap(image_size.cx, image_size.cy, fmt, WICBitmapCacheOnLoad, &t);
        return t;
    }

    inline IWICBitmapPtr CreateBitmapFromSource(IWICBitmapSource* src)
    {
        IWICBitmapPtr   t;
        if (src) { g_factory->CreateBitmapFromSource(src, WICBitmapCacheOnLoad, &t); }
        return t;
    }

    inline IWICBitmapPtr EnsureBitmapCreated(IWICBitmapSource* src)
    {
        if (IWICBitmapPtr t = src)
            return t;
        return CreateBitmapFromSource(src);
    }

    inline IWICBitmapPtr CreateBitmapFromHICON(HICON ico)
    {
        IWICBitmapPtr   t;
        if (ico) { g_factory->CreateBitmapFromHICON(ico, &t); }
        return t;
    }

    inline IWICBitmapPtr CreateBitmapFromHBITMAP(HBITMAP src, WICBitmapAlphaChannelOption options)
    {
        IWICBitmapPtr   t;
        if (src) { g_factory->CreateBitmapFromHBITMAP(src, NULL, options, &t); }
        return t;
    }

    inline IWICBitmapFlipRotatorPtr CreateBitmapFlipRotator()
    {
        IWICBitmapFlipRotatorPtr   t;
        g_factory->CreateBitmapFlipRotator(&t);
        return t;
    }

    // an extremely long-running operation caused by the FlipRotator. Caching the source in a bitmap before using FlipRotator will work around the issue.
    inline IWICBitmapSourcePtr FlipRotateBitmapSafe(IWICBitmapSource* src, WICBitmapTransformOptions flag)
    {
        if (IWICBitmapPtr bmp = EnsureBitmapCreated(src))
        {
            auto   cmd = CreateBitmapFlipRotator();
            if (cmd && (cmd->Initialize(bmp, flag) == S_OK))
                return cmd;
        }
        assert(false);
        return src;
    }

    inline IWICColorTransformPtr CreateColorTransformer()
    {
        IWICColorTransformPtr   t;
        g_factory->CreateColorTransformer(&t);
        return t;
    }

    inline IWICBitmapDecoderPtr CreateDecoderFromFilename(PCWSTR filepath, DWORD desired_access)
    {
        IWICBitmapDecoderPtr   t;
        g_factory->CreateDecoderFromFilename(filepath, NULL, desired_access, WICDecodeMetadataCacheOnDemand, &t);
        return t;
    }

    inline IWICBitmapDecoderPtr CreateDecoderFromStream(IStream* stm)
    {
        IWICBitmapDecoderPtr   t;
        if (stm) { g_factory->CreateDecoderFromStream(stm, NULL, WICDecodeMetadataCacheOnDemand, &t); }
        return t;
    }

    inline IWICBitmapDecoderPtr CreateDecoderFromFileNoLock(PCWSTR filepath)
    {
        auto   t = CreateStreamFromFileNoLock(filepath);
        return CreateDecoderFromStream(t);
    }

    // NO embedded ICC and JPEG orientation applied
    inline IWICBitmapPtr LoadPlainImageFromStream(IStream* stm, REFWICPixelFormatGUID output_format)
    {
        auto   decoder = CreateDecoderFromStream(stm);
        auto   frame = GetFrame(decoder, 0); // first frame
        auto   dest = ConvertFormat(frame, output_format); assert(dest);
        return CreateBitmapFromSource(dest);
    }
}
