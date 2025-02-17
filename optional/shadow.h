#pragma once

_PHOXO_BEGIN
_PHOXO_EFFECT_BEGIN

/// Shadow data
struct SHADOWDATA
{
    /// blur radius, default is 5
    int   m_smooth = 5;
    /// color of shadow, default is RGB(63,63,63)
    RGBA32bit   m_color = { 63, 63, 63, 0xFF };
    /// 0 <= opacity <= 100, default is 75
    int   m_opacity = 75;
    /// offset of shadow, default is 5
    CSize   m_offset = CSize(5, 5);

    CRect CalculateNewRect(CRect src) const
    {
        int   expand = (int)(2.7 * m_smooth);
        CRect   rc = src;
        rc.OffsetRect(m_offset);
        rc.InflateRect(expand, expand);

        CRect   ret;
        ret.UnionRect(src, rc);
        return ret;
    }
};

class FCEffectPrepareShadow : public FCImageEffect
{
private:
    CSize   m_new_size;
    const FCImage   & m_original;
    CSize   m_shadow_offset;
    SHADOWDATA   m_info;

public:
    FCEffectPrepareShadow(CSize new_size, const FCImage& original, CSize shadow_offset, SHADOWDATA info) : m_original(original)
    {
        m_new_size = new_size;
        m_shadow_offset = shadow_offset;
        m_info = info;
    }

    virtual bool IsSupport(const FCImage& img) { return true; }

    virtual void OnBeforeProcess(FCImage& img)
    {
        img.Create(m_new_size.cx, m_new_size.cy, 32);
    }

    virtual void ProcessPixel(FCImage& img, int x, int y, RGBA32bit* pixel)
    {
        CPoint   on_original = CPoint(x, y) - m_shadow_offset;
        if (m_original.IsInside(on_original))
        {
            auto   src = (RGBA32bit*)m_original.GetPixel(on_original);
            m_info.m_color.a = (BYTE)(src->a * m_info.m_opacity / 100);
        }
        else
        {
            m_info.m_color.a = 0;
        }
        *pixel = m_info.m_color;
    }
};

/// Add shadow (32 bit).
class FCEffectAddShadow : public FCImageEffect
{
private:
    SHADOWDATA   m_shadow ;

public:
    /// constructor.
    FCEffectAddShadow (SHADOWDATA sd)
    {
        m_shadow = sd ;
        m_shadow.m_opacity = std::clamp(m_shadow.m_opacity, 0, 100) ;
        m_shadow.m_offset.cx = std::clamp((int)m_shadow.m_offset.cx, -999, 999) ;
        m_shadow.m_offset.cy = std::clamp((int)m_shadow.m_offset.cy, -999, 999) ;
    }

private:
    virtual bool IsSupport(const FCImage& img)
    {
        return (img.ColorBits() == 32) && !img.IsPremultiplied();
    }

    virtual ProcessMode QueryProcessMode()
    {
        return ProcessMode::EntireMyself;
    }

    virtual void ProcessEntire(FCImage& img, FCProgressObserver* progress)
    {
        CRect   result_rect = m_shadow.CalculateNewRect(CRect(0, 0, img.Width(), img.Height()));

        FCImage   original;
        img.SwapImage(original);

        imsprivate::FCEffectPrepareShadow   cmd(result_rect.Size(), original, m_shadow.m_offset - result_rect.TopLeft(), m_shadow);
        cmd.EnableParallelAccelerate(IsParallelAccelerateEnable());
        img.ApplyEffect(cmd);

        // blur bottom shadow
        phoxo::effect::GaussianBlur   blur_cmd (m_shadow.m_smooth, false, true) ;
        blur_cmd.EnableParallelAccelerate(IsParallelAccelerateEnable());
        img.ApplyEffect (blur_cmd) ;

        // combine original image
        phoxo::effect::CompositeStraight   composite_cmd(original, CSize(0, 0) - result_rect.TopLeft());
        composite_cmd.EnableParallelAccelerate(IsParallelAccelerateEnable());
        img.ApplyEffect(composite_cmd);
    }
};

_PHOXO_NAMESPACE_END
_PHOXO_NAMESPACE_END
