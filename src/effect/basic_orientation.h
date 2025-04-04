#pragma once

_PHOXO_BEGIN
_PHOXO_EFFECT_BEGIN

/// Left right mirror (32 bit).
class Mirror : public PixelIterator<Mirror>
{
public:
    static void HandlePixel(Image& img, int x, int y, RGBA32bit* px, ImageEffect&)
    {
        if (x < img.Width() / 2)
        {
            CPoint   pt(img.Width() - 1 - x, y);
            std::swap(*px, *(RGBA32bit*)img.GetPixel(pt));
        }
    }
};

/// Top bottom flip (32 bit).
class Flip : public PixelIterator<Flip>
{
public:
    static void HandlePixel(Image& img, int x, int y, RGBA32bit* px, ImageEffect&)
    {
        if (y < img.Height() / 2)
        {
            CPoint   pt(x, img.Height() - 1 - y);
            std::swap(*px, *(RGBA32bit*)img.GetPixel(pt));
        }
    }
};

/// @cond
class _Rotate90Tool
{
private:
    Image   m_bak;
protected:
    void RInit(Image& img)
    {
        img.Swap(m_bak);
        img.Create(m_bak.Height(), m_bak.Width(), m_bak.ColorBits(), m_bak.GetAttribute());
    }

    RGBA32bit RMap(int x, int y) const
    {
        return *(RGBA32bit*)m_bak.GetPixel(x, y);
    }
};
/// @endcond

/// Rotate 90 degree clockwise (32 bit).
class Rotate90 : public PixelIterator<Rotate90>,
                 public _Rotate90Tool
{
private:
    void OnBeforeProcess(Image& img) override
    {
        RInit(img);
    }
public:
    static void HandlePixel(Image& img, int x, int y, RGBA32bit* px, Rotate90& eff)
    {
        *px = eff.RMap(y, img.Width() - 1 - x);
    }
};

/// Rotate 270 degree clockwise (32 bit).
class Rotate270 : public PixelIterator<Rotate270>,
                  public _Rotate90Tool
{
private:
    void OnBeforeProcess(Image& img) override
    {
        RInit(img);
    }
public:
    static void HandlePixel(Image& img, int x, int y, RGBA32bit* px, Rotate270& eff)
    {
        *px = eff.RMap(img.Height() - 1 - y, x);
    }
};

_PHOXO_NAMESPACE_END
_PHOXO_NAMESPACE_END
