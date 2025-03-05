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
        // �ȸı��ʽ����Ҫ
        // 1. ������һ�θ�ʽ��һ��apply ICCʱ����
        // 2. ��JPG, ���32bpp�����Է��ֶ���Ҫ��ת��jpg��ת��32bpp�ٶȸ���
        // 3. ת��Ŀ���ʽ���Һ������load��ʽһ�£�������ת��ʱ����Ҫʵ�廯
        IWICBitmapSourcePtr   dest = CWICFunc::ConvertFormat(frame_decode, output_format); // <-- ��ʽһ��

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

        return Load(dest, img, output_format); // <-- ��ʽһ��
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
                // ���ûװhevc ext, heif�ļ�CopyPixels����0xc00d5212
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

        // ��Ҫ�ı�icc���ظ�ʽ����һ�ν���.cr2��ʽ��������ʱ��
        auto   format = CWICFunc::GetPixelFormat(src_bmp);
        if (auto trans = CWICFunc::CreateColorTransformer())
        {
            if (trans->Initialize(src_bmp, src_icc, dest_icc, format) == S_OK)
                return trans;
        }
        // assert(false); �ص�assert��һЩgoogle sRGBҲת��ʧ��
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
