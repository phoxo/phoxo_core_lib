#pragma once
#include "define.h"
#include "image_effect.h"

_PHOXO_BEGIN

/**
    @brief Image object.

    (0,0) located at <span style='color:#FF0000'>upper-left</span>, x increase from left to right, y increase from top to down.
*/
class Image
{
public:
    enum
    {
        PremultipliedAlpha = 0x01,
    };

private:
    int   m_attribute = 0;
    int   m_width = 0;
    int   m_height = 0;
    int   m_bpp = 0;
    int   m_stride = 0;
    BYTE   * m_pixel = nullptr;
    HBITMAP   m_DIB_Handle = nullptr;

public:
    /// @name Constructor.
    ///@{
    /***/
    /// default constructor.
    Image() {}
    /// copy constructor.
    Image(const Image& img) { *this = img; }
    /// move constructor.
    Image(Image&& img)
    {
        SwapImage(img);
    }
    /// destructor.
    virtual ~Image() { Destroy(); }
    /// copy operator.
    Image& operator= (const Image& img)
    {
        if (&img == this) { assert(false); return *this; }
        if (!img) { Destroy(); return *this; }
        if (Create(img.Width(), img.Height(), img.ColorBits(), img.m_attribute))
        {
            memcpy(GetMemStart(), img.GetMemStart(), img.GetPixelBufferSize());
        }
        return *this;
    }
    /// move assignment operator.
    Image& operator=(Image&& other)
    {
        if (&other == this) { assert(false); return *this; }
        Destroy();
        SwapImage(other);
        return *this;
    }
    ///@}

    /// @name Create / Destroy.
    ///@{
    /***/
    /// create image, bpp can be <span style='color:#FF0000'>8 , 24 , 32</span>.
    bool Create(int width, int height, int bpp, int attribute = 0)
    {
        Destroy();
        if (!width || !height || !bpp)
        {
            assert(false); return false;
        }

        m_attribute = attribute;
        m_width = width;
        m_height = abs(height);
        m_bpp = bpp;
        m_stride = Math::CalcStride(width, bpp);
        AllocPixelBuffer();
        if (!m_pixel || (bpp == 1) || (bpp == 4) || (bpp == 16)) // unsupported format
        {
            Destroy();
            assert(false); return false;
        }
        return true;
    }

    /// destroy image.
    void Destroy()
    {
        if (m_DIB_Handle) { DeleteObject(m_DIB_Handle); }
        InitMember();
    }

    /// swap current image with img.
    void SwapImage(Image& img)
    {
        std::swap(img.m_attribute, m_attribute);
        std::swap(img.m_width, m_width);
        std::swap(img.m_height, m_height);
        std::swap(img.m_bpp, m_bpp);
        std::swap(img.m_pixel, m_pixel);
        std::swap(img.m_DIB_Handle, m_DIB_Handle);
        std::swap(img.m_stride, m_stride);
    }

    /// releases ownership of its DIB handle.
    HBITMAP Detach()
    {
        auto   bmp = m_DIB_Handle;
        InitMember();
        return bmp;
    }
    ///@}

    /// @name Attributes.
    ///@{
    /***/
    bool IsValid() const { return (m_pixel != 0); }
    bool IsInside(int x, int y) const { return (x >= 0) && (x < m_width) && (y >= 0) && (y < m_height); }
    bool IsInside(POINT pt) const { return IsInside(pt.x, pt.y); }

    /// this function doesn't do boundary check, so <span style='color:#FF0000'>Crash</span> if y exceed.
    BYTE* GetLinePtr(int y) const
    {
        assert(IsInside(0, y));
        return m_pixel + (y * m_stride);
    }
    BYTE* GetPixel(int x, int y) const
    {
        assert(IsInside(x, y));
        auto   py = GetLinePtr(y);
        if (m_bpp == 32)
            return (py + x * 4);
        if (m_bpp == 8)
            return (py + x);
        return (py + x * 3); // 24bpp
    }
    BYTE* GetPixel(POINT pt) const { return GetPixel(pt.x, pt.y); }

    SIZE GetSize() const { return CSize(m_width, m_height); }
    int Width() const { return m_width; }
    int Height() const { return m_height; }
    int ColorBits() const { return m_bpp; }
    int GetStride() const { return m_stride; }
    /// equal stride * height
    int GetPixelBufferSize() const { return (m_stride * Height()); }
    /// get start address of pixel.
    void* GetMemStart() const { return m_pixel; }
    int GetAttribute() const { return m_attribute; }
    operator HBITMAP() const { return m_DIB_Handle; }
    operator bool() const { return (m_pixel != 0); }
    bool IsPremultiplied() const { return (m_attribute & PremultipliedAlpha); }
    void SetPremultiplied(bool v) { v ? ModifyAttribute(0, PremultipliedAlpha) : ModifyAttribute(PremultipliedAlpha, 0); }
    ///@}

    /// @name Temporary object.
    ///@{
    /***/
    /// you can apply some effect (e.g. FCEffectBrightnessContrast) on buffer
    void Attach32bppBuffer(int width, int height, BYTE* pixel)
    {
        if ((width > 0) && (height != 0) && pixel)
        {
            Destroy();
            m_width = width;
            m_height = abs(height);
            m_bpp = 32;
            m_pixel = pixel;
            m_stride = Math::CalcStride(width, 32);
        }
        else { assert(false); }
    }
    ///@}

    /// @name Process.
    ///@{
    /***/
    /// apply an effect, more detail refer to ImageEffect.
    void ApplyEffect(ImageEffect& effect, IProgressListener* progress = nullptr)
    {
        if (!effect.IsSupported(*this)) { assert(false); return; }

        // before
        effect.OnBeforeProcess(*this);
        if (progress)
            progress->UpdateProgress(0);

        auto   type = effect.QueryProcessMode();
        if (type == ImageEffect::ProcessMode::EntireMyself)
        {
            effect.ProcessEntire(*this, progress);
        }
        else
        {
            if (effect.IsParallelEnabled())
            {
                ApplyEffectParallel(effect, progress);
            }
            else
            {
                effect.ProcessRegion(*this, CRect(0, 0, Width(), Height()), progress);
            }
        }

        effect.OnAfterProcess(*this);
        if (progress && !progress->IsUserCanceled())
            progress->UpdateProgress(100);
    }

    // 四个参数就不会inline，去掉最后一个参数就会内联，因此加上 __forceinline
    template<class FUNC, class EFFT>
    __forceinline void IterateRangePixels(CRect rc, EFFT& effect, FUNC handle_pixel, IProgressListener* progress = nullptr)
    {
        if (!handle_pixel) { assert(false); return; }

        int   bpp = ColorBits() / 8;
        for (int y = rc.top; y < rc.bottom; y++)
        {
            PBYTE   cur = GetPixel(rc.left, y);
            for (int x = rc.left; x < rc.right; x++, cur += bpp)
            {
                handle_pixel(*this, x, y, (RGBA32bit*)cur, effect);
            }

            // 没必要每行都更新进度，50行一次更新，给用户中断处理的机会
            if (progress && (y % 50 == 0) && !progress->UpdateProgress((y - rc.top) * 100 / rc.Height()))
                break;
        }
    }

    template<class T>
    void IterateRangePixels2(CRect rc, T& effect, IProgressListener* progress = nullptr)
    {
        IterateRangePixels(rc, effect, T::HandlePixel, progress);
    }
    ///@}

private:
    void ModifyAttribute(int remove, int add) { m_attribute = ((m_attribute & ~remove) | add); }

    void InitMember()
    {
        m_attribute = 0; m_width = 0; m_height = 0; m_bpp = 0; m_stride = 0;
        m_pixel = nullptr; m_DIB_Handle = nullptr;
    }

    void AllocPixelBuffer()
    {
        size_t   info_byte = sizeof(BITMAPINFOHEADER) + 16;
        if (ColorBits() <= 8)
        {
            info_byte += (sizeof(RGBQUAD) * 256);
        }

        std::vector<BYTE>   buf(info_byte, (BYTE)0);
        auto   info = (BITMAPINFO*)buf.data();

        Utils::InitBitmapInfoHeader(info->bmiHeader, m_width, m_height, m_bpp, true);
        m_DIB_Handle = CreateDIBSection(NULL, info, DIB_RGB_COLORS, (VOID**)&m_pixel, NULL, 0);
    }

    void ApplyEffectParallel(ImageEffect& effect, IProgressListener* progress);
};

_PHOXO_NAMESPACE_END
