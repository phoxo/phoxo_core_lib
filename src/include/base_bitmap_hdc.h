#pragma once

_PHOXO_BEGIN

/// Memory DC with auto bitmap select.
class BitmapHDC
{
private:
    HDC      m_dc = CreateCompatibleDC(NULL);
    HGDIOBJ  m_old;
    HGDIOBJ  m_font_bak;

public:
    /// create memory dc and select bmp in.
    BitmapHDC(HBITMAP bmp)
    {
        m_old = SelectObject(m_dc, bmp);
        m_font_bak = GetCurrentObject(m_dc, OBJ_FONT);
        SetBkMode(m_dc, TRANSPARENT);
        SetStretchBltMode(m_dc, COLORONCOLOR);
    }

    ~BitmapHDC()
    {
        SelectObject(m_dc, m_font_bak);
        SelectObject(m_dc, m_old);
        DeleteDC(m_dc);
    }

    /// get HDC handle.
    operator HDC() const { return m_dc; }
};

_PHOXO_NAMESPACE_END
