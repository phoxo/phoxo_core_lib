#pragma once
#include "wic_image_save.h"
#include "wic_locked_pixel_buffer.h"

_PHOXO_BEGIN

/// Read / Write image using WIC
class CodecWIC
{
public:
    static bool LoadFile(PCWSTR filepath, Image& img, REFWICPixelFormatGUID output_format = WICNormal32bpp, CWICMetadata* meta = NULL, bool use_embedded_icc = false)
    {
        auto   stm = CWICFunc::CreateStreamFromFileNoLock(filepath);
        return LoadStream(stm, img, output_format, meta, use_embedded_icc);
    }

    static bool LoadStream(IStream* stm, Image& img, REFWICPixelFormatGUID output_format, CWICMetadata* meta = NULL, bool use_embedded_icc = false)
    {
        auto   decoder = CWICFunc::CreateDecoderFromStream(stm);
        auto   first_frame = CWICFunc::GetFrame(decoder, 0); // just load first frame

        if (meta)
        {
            meta->Read(first_frame);
        }
        return LoadFrame(first_frame, img, output_format, use_embedded_icc);
    }

    static bool LoadFrame(IWICBitmapFrameDecode* frame_decode, Image& img, REFWICPixelFormatGUID output_format, bool use_embedded_icc = false)
    {
        // 先改变格式很重要
        // 1. 碰到过一次格式不一样apply ICC时卡死
        // 2. 读JPG, 输出32bpp，测试发现对需要旋转的jpg先转换32bpp速度更快
        // 3. 转到目标格式，且和下面的load格式一致，则在旋转的时候不需要实体化
        IWICBitmapSourcePtr   dest = CWICFunc::ConvertFormat(frame_decode, output_format); // <-- 格式一致

        if (use_embedded_icc)
        {
            if (auto icc = CWICFunc::GetFirstColorContext(frame_decode))
            {
                dest = ApplyEmbeddedICC(dest, icc);
            }
        }

        static_assert(WICBitmapTransformRotate0 == 0);
        if (auto rotate = CWICMetadataOrientation::Read(frame_decode))
        {
            dest = CorrectOrientation(dest, rotate);
        }

        return Load(dest, img, output_format); // <-- 格式一致
    }

    static bool Load(IWICBitmapSource* src_bmp, Image& img, WICPixelFormatGUID output_format)
    {
        int   bpp = CWICFunc::GetBitsPerPixel(output_format);
        int   attr = (output_format == WICPremultiplied32bpp) ? Image::PremultipliedAlpha : 0;
        if (auto src = CWICFunc::ConvertFormat(src_bmp, output_format))
        {
            CSize   sz = CWICFunc::GetBitmapSize(src);
            if (img.Create(sz.cx, sz.cy, bpp, attr))
            {
                if (src->CopyPixels(NULL, img.GetStride(), img.GetPixelBufferSize(), (BYTE*)img.GetMemStart()) == S_OK)
                    return true;
                // 如果没装hevc ext, heif文件CopyPixels返回0xc00d5212
            }
        }
        assert(false);
        img.Destroy();
        return false;
    }

    static IWICBitmapSourcePtr ApplyEmbeddedICC(IWICBitmapSourcePtr src_bmp, IWICColorContextPtr src_icc, bool restore_from_srgb = false)
    {
        auto   dest_icc = CWICFunc::CreateSystemColorContext_SRGB();
        if (restore_from_srgb)
        {
            std::swap(src_icc, dest_icc);
        }

        // 不要改变icc像素格式，有一次解码.cr2格式遇到超长时间
        auto   format = CWICFunc::GetPixelFormat(src_bmp);
        if (auto trans = CWICFunc::CreateColorTransformer())
        {
            if (trans->Initialize(src_bmp, src_icc, dest_icc, format) == S_OK)
                return trans;
        }
        // assert(false); 关掉assert，一些google sRGB也转换失败
        return src_bmp;
    }

private:
    static IWICBitmapSourcePtr CorrectOrientation(IWICBitmapSourcePtr src, WICBitmapTransformOptions flag)
    {
        if (auto cmd = CWICFunc::CreateBitmapFlipRotator())
        {
            if (cmd->Initialize(src, flag) == S_OK)
                return cmd;
        }
        assert(false);
        return src;
    }
};

_PHOXO_NAMESPACE_END
