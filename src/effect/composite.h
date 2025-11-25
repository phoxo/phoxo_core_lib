#pragma once

_PHOXO_BEGIN
_PHOXO_EFFECT_BEGIN

/// Composite two 32-bit straight alpha images (32 bit).
class CompositeStraight : public ImageEffect
{
private:
    const Image   & m_top;
    const POINT   m_top_pos;
    const int   m_percent;

public:
    CompositeStraight(const Image& top, POINT top_position, int top_percent = 100)
        : m_top(top)
        , m_top_pos(top_position)
        , m_percent(std::clamp(top_percent, 0, 100))
    {
    }

private:
    bool IsSupported(const Image& img) override
    {
        return (img.ColorBits() == 32) && (m_top.ColorBits() == 32) && !img.IsPremultiplied() && !m_top.IsPremultiplied();
    }

    void ProcessRegion(Image& img, CRect region_rect, IProgressListener* progress) override
    {
        CRect   rect_top(m_top_pos, m_top.Size());
        CRect   rc;
        if (!rc.IntersectRect(region_rect, rect_top))
            return;

        for (int y = rc.top; y < rc.bottom; y++)
        {
            auto   dst = (Color*)img.GetPixel(rc.left, y);
            auto   src = (Color*)m_top.GetPixel(rc.left - m_top_pos.x, y - m_top_pos.y);
            for (int x = rc.left; x < rc.right; x++, dst++, src++)
            {
                Color   cr = *src;
                if (m_percent != 100)
                {
                    cr.a = (BYTE)((cr.a * m_percent + 50) / 100);
                }
                PixelFunc::CompositeStraightAlpha(*dst, cr);
            }
        }
    }
};

_PHOXO_NAMESPACE_END
_PHOXO_NAMESPACE_END
