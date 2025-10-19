#pragma once
#include "wic_image_save.h"

_PHOXO_BEGIN

/// Read / Write image using WIC
class CodecWIC
{
public:
    static Image LoadFile(PCWSTR filepath, REFWICPixelFormatGUID output_format = WICNormal32bpp, CWICMetadata* meta = NULL, bool use_embedded_icc = false)
    {
        auto   stm = WIC::CreateStreamFromFileNoLock(filepath);
        return LoadStream(stm, output_format, meta, use_embedded_icc);
    }

    static Image LoadStream(IStream* stream, REFWICPixelFormatGUID output_format, CWICMetadata* meta = NULL, bool use_embedded_icc = false)
    {
        auto   decoder = WIC::CreateDecoderFromStream(stream);
        auto   first_frame = WIC::GetFrame(decoder, 0); // just load first frame
        if (meta)
        {
            meta->Read(first_frame);
        }
        return LoadFrame(first_frame, output_format, use_embedded_icc);
    }

    static Image LoadFrame(IWICBitmapFrameDecode* frame_decode, REFWICPixelFormatGUID output_format, bool use_embedded_icc = false)
    {
        // 先改变格式很重要
        // 1. 碰到过一次格式不一样apply ICC时卡死
        // 2. 读JPG, 输出32bpp，测试发现对需要旋转的jpg先转换32bpp速度更快
        // 3. 先转换格式，且和下面的load格式一致，则在矫正方向时候不需要实体化。24位jpg，如果直接矫正方向，后转换格式速度会极其慢
        IWICBitmapSourcePtr   dest = WIC::ConvertFormat(frame_decode, output_format); // <-- 格式一致

        if (use_embedded_icc)
        {
            if (auto icc = WIC::GetFirstColorContext(frame_decode))
            {
                dest = ApplyEmbeddedICC(dest, icc);
            }
        }

        if (auto rotate = WIC::OrientationTag::Read(frame_decode))
        {
            dest = CorrectOrientation(dest, rotate);
        }

        return ImageHandler::Make(dest, output_format); // <-- 格式要一致
    }

    static IWICBitmapSourcePtr ApplyEmbeddedICC(IWICBitmapSource* src_bmp, IWICColorContextPtr src_icc, bool restore_from_srgb = false)
    {
        auto   dest_icc = WIC::CreateSystemColorContext_SRGB();
        if (restore_from_srgb)
        {
            std::swap(src_icc, dest_icc);
        }

        // 不要改变icc像素格式，有一次解码.cr2格式遇到超长时间
        auto   format = WIC::GetPixelFormat(src_bmp);
        if (auto trans = CreateColorTransformer())
        {
            if (trans->Initialize(src_bmp, src_icc, dest_icc, format) == S_OK)
                return trans;
        }
        // assert(false); 关掉assert，一些google sRGB也转换失败
        return src_bmp;
    }

private:
    static IWICBitmapSourcePtr CorrectOrientation(IWICBitmapSource* src, WICBitmapTransformOptions flag)
    {
        if (auto cmd = WIC::CreateBitmapFlipRotator())
        {
            if (cmd->Initialize(src, flag) == S_OK)
                return cmd;
        }
        assert(false);
        return src;
    }

    static IWICColorTransformPtr CreateColorTransformer()
    {
        IWICColorTransformPtr   t;
        WIC::g_factory->CreateColorTransformer(&t);
        return t;
    }
};

_PHOXO_NAMESPACE_END
