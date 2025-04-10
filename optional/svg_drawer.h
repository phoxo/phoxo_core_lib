#pragma once
#include "render_target.h"

namespace D2D
{
    class SVGDrawer
    {
    private:
        IStreamPtr   m_stream;
        ID2D1FactoryPtr   m_factory;

    public:
        SVGDrawer(IStream* svg_stream, ID2D1Factory* factory)
        {
            m_stream = svg_stream;
            m_factory = factory;
        }

        // 注意：输出格式是 premultiplied alpha
        IWICBitmapPtr CreateBitmap(SIZE output_size, float scale) const
        {
            // from Windows 10 1703, build 15063. 版本太老返回NULL
            auto   bmp = WIC::CreateBitmap(output_size, WICPremultiplied32bpp); assert(bmp);
            ID2D1DeviceContext5Ptr   dc5 = CreateWicBitmapRenderTarget(m_factory, bmp); // <== 有隐式转换
            if (dc5 && m_stream)
            {
                if (scale != 1.0f)
                {
                    dc5->SetTransform(D2D1::Matrix3x2F::Scale(scale, scale));
                }

                if (DrawSvgDocument(dc5, output_size))
                    return bmp;
            }
            return nullptr;
        }

    private:
        bool DrawSvgDocument(ID2D1DeviceContext5* dc, SIZE output_size) const
        {
            D2D1_SIZE_F   view_size{ (float)output_size.cx, (float)output_size.cy };
            ID2D1SvgDocumentPtr   svg;
            dc->CreateSvgDocument(m_stream, view_size, &svg);
            if (svg)
            {
                dc->BeginDraw();
                dc->DrawSvgDocument(svg);
                return dc->EndDraw() == S_OK;
            }
            return false;
        }
    };
}
