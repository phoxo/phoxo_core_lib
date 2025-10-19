
class LocalRenderer : public IDWriteTextRenderer
{
private:
    ID2D1RenderTargetPtr   m_target;
    ID2D1SolidColorBrushPtr   m_brush;
    ID2D1FactoryPtr   m_factory;
    float   m_stroke_width = 1;

public:
    LocalRenderer(ID2D1RenderTarget* target) : m_target(target)
    {
        target->CreateSolidColorBrush(D2D1::ColorF(0), &m_brush);
        target->GetFactory(&m_factory);
    }

private:
    HRESULT DrawGlyphRun(void*, FLOAT baselineOriginX, FLOAT baselineOriginY,
        DWRITE_MEASURING_MODE,
        const DWRITE_GLYPH_RUN* run,
        const DWRITE_GLYPH_RUN_DESCRIPTION* desc,
        IUnknown* clientDrawingEffect)
    {
        try
        {
            ID2D1PathGeometryPtr   geometry;
            ID2D1GeometrySinkPtr   sink;
            m_factory->CreatePathGeometry(&geometry);
            geometry->Open(&sink);

            run->fontFace->GetGlyphRunOutline(
                run->fontEmSize,
                run->glyphIndices,
                run->glyphAdvances,
                run->glyphOffsets,
                run->glyphCount,
                run->isSideways,
                run->bidiLevel % 2,
                sink
            );
            sink->Close();

            ID2D1TransformedGeometryPtr   formed;
            m_factory->CreateTransformedGeometry(geometry, D2D1::Matrix3x2F::Translation(baselineOriginX, baselineOriginY), &formed);
            if (formed && m_brush)
            {
                m_target->DrawGeometry(formed, m_brush, m_stroke_width);
            }

            // FillGeometry£¨xformedGeometry.Get£¨£©£¬ brush£©;
        }
        catch (_com_error&) {}
        return S_OK;
    }

    HRESULT DrawUnderline(void*, FLOAT, FLOAT, DWRITE_UNDERLINE const*, IUnknown*) { return E_NOTIMPL; }
    HRESULT DrawStrikethrough(void*, FLOAT, FLOAT, DWRITE_STRIKETHROUGH const*, IUnknown*) { return E_NOTIMPL; }
    HRESULT DrawInlineObject(void*, FLOAT, FLOAT, IDWriteInlineObject*, BOOL, BOOL, IUnknown*) { return E_NOTIMPL; }

    HRESULT IsPixelSnappingDisabled(void*, BOOL* disabled)
    {
        *disabled = FALSE;
        return S_OK;
    }

    HRESULT GetCurrentTransform(void*, DWRITE_MATRIX* transform)
    {
        m_target->GetTransform((D2D1_MATRIX_3X2_F*)transform);
        return S_OK;
    }

    HRESULT GetPixelsPerDip(void*, FLOAT* pixelsPerDip)
    {
        D2D_POINT_2F   dpi{};
        m_target->GetDpi(&dpi.x, &dpi.y);
        *pixelsPerDip = dpi.x / 96;
        return S_OK;
    }

    ULONG AddRef() { return 1; }
    ULONG Release() { return 1; }
    HRESULT QueryInterface(REFIID, void** ppv) { *ppv = nullptr; return E_NOINTERFACE; }
};



auto   memdc = make_unique<BitmapHDC>(img);
auto   target = D2D::CreateDCRenderTarget(D2D1_RENDER_TARGET_TYPE_SOFTWARE, *memdc);
target->BeginDraw();
target->Clear({ 0,0,0,0 });
// 
LocalRenderer renderer(target);
layout->Draw(NULL, &renderer, 0, 0);
target->EndDraw();
