#pragma once

_PHOXO_BEGIN
_PHOXO_EFFECT_BEGIN

/// Fill grid (32 bit).
class CheckerFill : public PixelIterator<CheckerFill>
{
private:
    RGBA32bit   m_cr1, m_cr2;
    int   m_size;

public:
    /// grid_size - width of grid, >=1
    CheckerFill(Color cr1, Color cr2, int grid_size, int alpha_value = 0xFF) : m_cr1(cr1), m_cr2(cr2)
    {
        m_size = (std::max)(grid_size, 1);
        m_cr1.a = m_cr2.a = Math::Clamp0255(alpha_value);
    }

private:
    void Process(int x, int y, RGBA32bit& px) const
    {
        int   nX = x / m_size, nY = y / m_size;
        RGBA32bit   c = ((nX + nY) % 2 == 0) ? m_cr1 : m_cr2;
        Color::CompositeStraightAlpha(px, c);
    }

public:
    static void HandlePixel(Image&, int x, int y, RGBA32bit* px, CheckerFill& eff)
    {
        eff.Process(x, y, *px);
    }
};

_PHOXO_NAMESPACE_END
_PHOXO_NAMESPACE_END
