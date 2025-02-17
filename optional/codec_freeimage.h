#pragma once

/// Read / Write image using FreeImage library.
class FCCodecFreeImage
{
public:
    struct CAutoFIBITMAP
    {
        FIBITMAP   * m_bmp;
        operator FIBITMAP*() const { return m_bmp; }
        CAutoFIBITMAP(FIBITMAP* src) : m_bmp(src) {}
        ~CAutoFIBITMAP() { if (m_bmp) { FreeImage_Unload(m_bmp); } }

        void ConvertTo32bpp()
        {
            if (FreeImage_GetBPP(m_bmp) == 32)
                return;
            auto   bmp32 = FreeImage_ConvertTo32Bits(m_bmp);
            FreeImage_Unload(m_bmp);
            m_bmp = bmp32;
        }
    };

public:
    /// load image.
    static bool Load(PCWSTR image_path, FCImage& img, int& dpi)
    {
        auto   img_type = FreeImage_GetFileTypeU(image_path);
        if (img_type != FIF_UNKNOWN)
        {
            CAutoFIBITMAP   fi_img(FreeImage_LoadU(img_type, image_path));
            dpi = (int)(FreeImage_GetDotsPerMeterX(fi_img) / 39.3700787 + 0.5);
            fi_img.ConvertTo32bpp();
            if (fi_img)
            {
                img.Create(FreeImage_GetWidth(fi_img), FreeImage_GetHeight(fi_img), FreeImage_GetBPP(fi_img));
                if (img)
                {
                    FreeImage_FlipVertical(fi_img);
                    memcpy(img.GetMemStart(), FreeImage_GetBits(fi_img), img.GetPixelBufferSize());
                    return true;
                }
            }
        }
        assert(false);
        return false;
    }

    /// save image.
    static bool Save(PCWSTR image_path, const FCImage& img, int dpi = 0)
    {
        if (img.ColorBits() >= 24)
        {
            CAutoFIBITMAP   fi_img(AllocateFreeImage(img));
            if (fi_img && dpi)
            {
                FreeImage_SetDotsPerMeterX(fi_img, (int)(dpi * 39.3700787 + 0.5));
                FreeImage_SetDotsPerMeterY(fi_img, (int)(dpi * 39.3700787 + 0.5));
            }
            auto   fmt = FreeImage_GetFIFFromFilenameU(image_path);
            return (bool)FreeImage_SaveU(fmt, fi_img, image_path);
        }
        assert(false);
        return false;
    }

private:
    static FIBITMAP* AllocateFreeImage(const FCImage& img)
    {
        auto   fib = FreeImage_Allocate(img.Width(), img.Height(), img.ColorBits());
        if (fib && (FreeImage_GetPitch(fib) == img.GetStride()))
        {
            memcpy(FreeImage_GetBits(fib), img.GetMemStart(), img.GetPixelBufferSize());
            FreeImage_FlipVertical(fib);
        }
        return fib;
    }
};
