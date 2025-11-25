#pragma once

_PHOXO_BEGIN

class GdiplusTextRender
{
public:
    struct StyleParams
    {
        float   point_size = 12;
        int   dpi = USER_DEFAULT_SCREEN_DPI;
        Gdiplus::FontStyle   style = Gdiplus::FontStyleRegular;

        float PixelSize() const
        {
            return point_size * dpi / 72.0f;
        }

        auto GetTextHint() const
        {
            if ((dpi == 96) && (point_size < 14))
                return Gdiplus::TextRenderingHintAntiAliasGridFit;
            else
                return Gdiplus::TextRenderingHintAntiAlias;
        }
    };

public:
    GdiplusTextRender(const CString& text, PCWSTR fontname, const StyleParams& params)
        : m_text(text)
        , m_params(params)
        , m_font(fontname, params.PixelSize(), params.style, Gdiplus::UnitPixel)
        , m_format(Gdiplus::StringFormat::GenericDefault())
    {
    }

    void SetMaxWidth(int width)
    {
        m_max_width = (float)width;
    }

    Image CreateTextImage(const Gdiplus::Brush& brush, int margin = 0) const
    {
        CSize   sz = MeasureText() + CSize(margin * 2, margin * 2);
        Image   img;
        img.Create(sz);
        img.ZeroPixels();
        if (auto refbmp = GdiplusUtils::CreateBitmapReference(img))
        {
            auto   g = CreateGraphics(*refbmp);
            g->DrawString(m_text, -1, &m_font, LayoutRect(margin), &m_format, &brush);
        }
        return img;
    }

    unique_ptr<GdiplusPathRender> CreateStrokeRender(int margin = 0) const
    {
        Gdiplus::FontFamily   family;
        m_font.GetFamily(&family);

        auto   obj = make_unique<GdiplusPathRender>();
        obj->m_path.AddString(m_text, -1, &family, m_params.style, m_params.PixelSize(), LayoutRect(margin), &m_format);
        return obj;
    }

    CSize MeasureText() const
    {
        Gdiplus::SizeF   result;
        Gdiplus::Bitmap   bmp(1, 1);
        if (auto g = CreateGraphics(bmp))
        {
            g->MeasureString(m_text, -1, &m_font, { m_max_width, 0 }, &m_format, &result); assert(result.Width > 0 && result.Height > 0);
        }
        return { (int)ceil(result.Width), (int)ceil(result.Height) };
    }

    unique_ptr<Gdiplus::Graphics> CreateGraphics(Gdiplus::Bitmap& bmp) const
    {
        using namespace Gdiplus;

        auto   g = make_unique<Graphics>(&bmp);
        g->SetPageUnit(UnitPixel);
        g->SetTextRenderingHint(m_params.GetTextHint());
        g->SetPixelOffsetMode(PixelOffsetModeHalf);
        g->SetSmoothingMode(SmoothingModeAntiAlias);
        g->SetCompositingQuality(CompositingQualityHighQuality);
        return g;
    }

private:
    Gdiplus::RectF LayoutRect(int margin) const
    {
        return { (float)margin, (float)margin, m_max_width, 0 };
    }

private:
    const CString   m_text;
    const StyleParams   m_params;
    const Gdiplus::Font   m_font;
    const Gdiplus::StringFormat   m_format;
    float   m_max_width = 0;
};

_PHOXO_NAMESPACE_END
