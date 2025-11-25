#pragma once

_PHOXO_BEGIN
_PHOXO_EFFECT_BEGIN

/// LUT(look up table) transformation (32 bit).
class ColorLUT : public PixelIterator<ColorLUT>
{
private:
    BYTE   m_LUT[256] = {};

protected:
    /// 0 <= idx <= 0xFF
    virtual BYTE InitLUT(int idx) = 0;

private:
    void OnBeforeProcess(Image& img) override
    {
        for (int i = 0; i <= 0xFF; i++)
        {
            m_LUT[i] = InitLUT(i);
        }
    }

    void Process(Color& x) const
    {
        x.b = m_LUT[x.b];
        x.g = m_LUT[x.g];
        x.r = m_LUT[x.r];
    }

public:
    static void HandlePixel(Image&, int, int, Color* px, ColorLUT& eff)
    {
        eff.Process(*px);
    }
};

_PHOXO_NAMESPACE_END
_PHOXO_NAMESPACE_END
