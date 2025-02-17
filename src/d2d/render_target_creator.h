#pragma once
#include <d2d1_3.h>

_COM_SMARTPTR_TYPEDEF(ID2D1Factory, __uuidof(ID2D1Factory));
_COM_SMARTPTR_TYPEDEF(ID2D1DCRenderTarget, __uuidof(ID2D1DCRenderTarget));
_COM_SMARTPTR_TYPEDEF(ID2D1RenderTarget, __uuidof(ID2D1RenderTarget));
_COM_SMARTPTR_TYPEDEF(ID2D1Bitmap, __uuidof(ID2D1Bitmap));
_COM_SMARTPTR_TYPEDEF(ID2D1DeviceContext, __uuidof(ID2D1DeviceContext));
_COM_SMARTPTR_TYPEDEF(ID2D1DeviceContext5, __uuidof(ID2D1DeviceContext5));
_COM_SMARTPTR_TYPEDEF(ID2D1SvgDocument, __uuidof(ID2D1SvgDocument));

class D2DRenderTargetCreator
{
private:
    static auto BuildProperties(D2D1_RENDER_TARGET_TYPE type)
    {
        auto   pixel_fmt = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
        return D2D1::RenderTargetProperties(type, pixel_fmt);
    }

public:
    static auto CreateDCRenderTarget(ID2D1Factory* factory, D2D1_RENDER_TARGET_TYPE type)
    {
        auto   prop = BuildProperties(type);
        ID2D1DCRenderTargetPtr   ret;
        factory->CreateDCRenderTarget(&prop, &ret);
        return ret;
    }

    static auto CreateWicBitmapRenderTarget(ID2D1Factory* factory, IWICBitmap* bmp)
    {
        auto   prop = BuildProperties(D2D1_RENDER_TARGET_TYPE_SOFTWARE);
        ID2D1RenderTargetPtr   ret;
        if (bmp) // 传给D2D NULL竟然会crash...
        {
            factory->CreateWicBitmapRenderTarget(bmp, &prop, &ret);
        }
        return ret;
    }
};

// class D2DGlobalEnvironment
// {
// public:
//     static inline ID2D1FactoryPtr   g_factory;
// };
