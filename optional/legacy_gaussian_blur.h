#pragma once

_PHOXO_BEGIN
_PHOXO_EFFECT_BEGIN

/// @cond
class BoxBlurAccumulator
{
private:
    int   m_radius;
    bool   m_copy_edge_pixel;
    bool   m_only_blur_alpha;
    int   m_kernel_length;

public:
    BoxBlurAccumulator(int radius, bool copy_edge_pixel, bool only_blur_alpha)
    {
        m_radius = radius;
        m_copy_edge_pixel = copy_edge_pixel;
        m_only_blur_alpha = only_blur_alpha;
        m_kernel_length = 2 * m_radius + 1;
    }

    void VerticalGetLine(const FCImage& img, int x, RGBA32bit* curr) const
    {
        FillPadding(curr, img.GetPixel(x, 0));
        for (int y = 0; y < img.Height(); y++)
        {
            *curr++ = *(RGBA32bit*)img.GetPixel(x, y);
        }
        FillPadding(curr, img.GetPixel(x, img.Height() - 1));
    }

    void HorizontalGetLine(const FCImage& img, int y, RGBA32bit* curr) const
    {
        FillPadding(curr, img.GetPixel(0, y));
        for (int x = 0; x < img.Width(); x++)
        {
            *curr++ = *(RGBA32bit*)img.GetPixel(x, y);
        }
        FillPadding(curr, img.GetPixel(img.Width() - 1, y));
    }

    auto CreateLineBuffer(int width_or_height) const
    {
        int   cnt = width_or_height + 2 * m_radius + 4; // +4 just for safety
        return std::vector<RGBA32bit>(cnt);
    }

    void BlurScanLine(RGBA32bit* dest, int pixel_span, const RGBA32bit* line, int width_or_height) const
    {
        float   sr = 0, sg = 0, sb = 0, sa = 0;
        AccumulateFirstKernel(line, sr, sg, sb, sa);

        auto   begin = line, end = line + m_kernel_length;
        for (int i = 0; i < width_or_height; i++, begin++, end++, dest += pixel_span)
        {
            if (m_only_blur_alpha)
            {
                dest->a = (BYTE)(sa / m_kernel_length + 0.5f);
                sa += end->a - begin->a;
            }
            else
            {
                if (sa > 0)
                {
                    dest->b = (BYTE)(sb / sa + 0.5f);
                    dest->g = (BYTE)(sg / sa + 0.5f);
                    dest->r = (BYTE)(sr / sa + 0.5f);
                    dest->a = (BYTE)(sa / m_kernel_length + 0.5f);
                }
                else
                {
                    *(int32_t*)dest = 0;
                }

                sb += (end->b * end->a) - (begin->b * begin->a);
                sg += (end->g * end->a) - (begin->g * begin->a);
                sr += (end->r * end->a) - (begin->r * begin->a);
                sa += end->a - begin->a;
            }
        }
    }

private:
    void AccumulateFirstKernel(const RGBA32bit* px, float& sr, float& sg, float& sb, float& sa) const
    {
        for (int i = 0; i < m_kernel_length; i++, px++)
        {
            if (m_only_blur_alpha)
            {
                sa += px->a;
            }
            else
            {
                sb += (px->b * px->a);
                sg += (px->g * px->a);
                sr += (px->r * px->a);
                sa += px->a;
            }
        }
    }

    void FillPadding(RGBA32bit*& curr, const void* edge_pixel) const
    {
        auto   t = (m_copy_edge_pixel ? *(RGBA32bit*)edge_pixel : RGBA32bit{});
        curr = std::fill_n(curr, m_radius, t);
    }
};

class BoxBlurVert : public FCImageEffect
{
private:
    const BoxBlurAccumulator   m_acc;
public:
    BoxBlurVert(auto& acc) : m_acc(acc) {}
private:
    virtual SIZE QueryScanLineCountEachParallelTask(const FCImage& img) { return CSize(200, 0); }
    virtual bool IsSupport(const FCImage& img) { return true; }
    virtual ProcessMode QueryProcessMode() { return ProcessMode::Region; }

    virtual void ProcessRegion(FCImage& img, CRect rc, IProgressListener* progress)
    {
        // Since this function may be called from other threads, it requires its own scan line buffer.
        auto   rowbuf = m_acc.CreateLineBuffer(img.Height());
        auto   ptr = rowbuf.data();

        for (int x = rc.left; x < rc.right; x++)
        {
            m_acc.VerticalGetLine(img, x, ptr);
            m_acc.BlurScanLine((RGBA32bit*)img.GetPixel(x, 0), img.Width(), ptr, img.Height());
            if (progress && !progress->UpdateProgress(0)) // need call it for cancel
                break;
        }
    }
};

class BoxBlurHoriz : public FCImageEffect
{
private:
    const BoxBlurAccumulator   m_acc;
public:
    BoxBlurHoriz(auto& acc) : m_acc(acc) {}
private:
    virtual SIZE QueryScanLineCountEachParallelTask(const FCImage& img) { return CSize(0, 200); }
    virtual bool IsSupport(const FCImage& img) { return true; }
    virtual ProcessMode QueryProcessMode() { return ProcessMode::Region; }

    virtual void ProcessRegion(FCImage& img, CRect rc, IProgressListener* progress)
    {
        auto   rowbuf = m_acc.CreateLineBuffer(img.Width());
        auto   ptr = rowbuf.data();

        for (int y = rc.top; y < rc.bottom; y++)
        {
            m_acc.HorizontalGetLine(img, y, ptr);
            m_acc.BlurScanLine((RGBA32bit*)img.GetLinePtr(y), 1, ptr, img.Width());
            if (progress && !progress->UpdateProgress(0))
                break;
        }
    }
};
/// @endcond

// Based on an article of Ivan Kuckir: http://blog.ivank.net/fastest-gaussian-blur.html
/// Gaussian blur (32 bit).
class GaussianBlur : public FCImageEffect
{
private:
    int   m_radius;
    bool   m_copy_edge_pixel;
    bool   m_only_blur_alpha;

public:
    GaussianBlur(int radius, bool copy_edge_pixel, bool only_blur_alpha = false)
    {
        m_radius = std::clamp(radius, 1, 500);
        m_copy_edge_pixel = copy_edge_pixel;
        m_only_blur_alpha = only_blur_alpha;
    }

private:
    static auto boxesForGauss(double sigma, int n) // standard deviation, number of boxes
    {
        double   wIdeal = sqrt((12 * sigma * sigma / n) + 1); // Ideal averaging filter width 
        int   wl = (int)floor(wIdeal);
        if (wl % 2 == 0) { wl--; }
        int   wu = wl + 2;
        double   mIdeal = (12 * sigma*sigma - n * wl*wl - 4 * n*wl - 3 * n) / (-4 * wl - 4);
        int   m = (int)round(mIdeal);

        std::vector<int>   ret;
        for (int i = 0; i < n; i++) { ret.push_back(i < m ? wl : wu); }
        return ret;
    }

    auto CreateBoxBlurGroup(const FCImage& img) const
    {
        auto   sub_boxes = boxesForGauss(m_radius, 3);

        std::vector<std::unique_ptr<FCImageEffect>>   ret;
        for (int iter : sub_boxes)
        {
            int   r = (iter - 1) / 2;
            if (r >= 1)
            {
                BoxBlurAccumulator   acc(r, m_copy_edge_pixel, m_only_blur_alpha);
                ret.push_back(std::make_unique<BoxBlurVert>(acc));
                ret.push_back(std::make_unique<BoxBlurHoriz>(acc));
            }
        }
        return ret;
    }

    ProcessMode QueryProcessMode() override
    {
        return ProcessMode::EntireMyself;
    }

    void ProcessEntire(FCImage& img, IProgressListener* progress) override
    {
        if (progress) { progress->BeginFixProgress(0); }

        auto   effect_group = CreateBoxBlurGroup(img);
        for (auto& eff : effect_group)
        {
            eff->EnableParallelAccelerate(IsParallelAccelerateEnable());
            img.ApplyEffect(*eff, progress);
        }

        if (progress) { progress->EndFixProgress(); }
    }
};

_PHOXO_NAMESPACE_END
_PHOXO_NAMESPACE_END
