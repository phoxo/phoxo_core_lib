#pragma once

_PHOXO_BEGIN

class GdiplusPathRender
{
public:
    struct PenParams
    {
        Color   color;
        float   width;
    };

public:
    void Draw(Gdiplus::Graphics& g, const PenParams& line)
    {
        g.DrawPath(StrokePen(line), &m_path);
    }

    // 2 <= steps <= 256
    void DrawGlow(Gdiplus::Graphics& g, int steps, Color clr)
    {
        if (!steps)
            return;

        clr.a = (BYTE)(256 / steps);
        for (int i = 1; i < steps; i++)
        {
            Draw(g, { clr, (float)i });
        }
    }

    void Fill(Gdiplus::Graphics& g, const Gdiplus::Brush* brush) const
    {
        g.FillPath(brush, &m_path);
    }

private:
    const auto* StrokePen(const PenParams& style)
    {
        if (!m_pen)
        {
            m_pen = make_unique<Gdiplus::Pen>(style.color, style.width);
            m_pen->SetLineJoin(Gdiplus::LineJoinRound);
        }
        else
        {
            m_pen->SetColor(style.color);
            m_pen->SetWidth(style.width);
        }
        return m_pen.get();
    }

public:
    Gdiplus::GraphicsPath   m_path;
private:
    unique_ptr<Gdiplus::Pen>   m_pen; // for draw outline path
};

_PHOXO_NAMESPACE_END
