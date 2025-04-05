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
        // �ȸı��ʽ����Ҫ
        // 1. ������һ�θ�ʽ��һ��apply ICCʱ����
        // 2. ��JPG, ���32bpp�����Է��ֶ���Ҫ��ת��jpg��ת��32bpp�ٶȸ���
        // 3. ��ת����ʽ���Һ������load��ʽһ�£����ڽ�������ʱ����Ҫʵ�廯��24λjpg�����ֱ�ӽ������򣬺�ת����ʽ�ٶȻἫ����
        IWICBitmapSourcePtr   dest = WIC::ConvertFormat(frame_decode, output_format); // <-- ��ʽһ��

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

        return ImageHandler::Make(dest, output_format); // <-- ��ʽҪһ��
    }

    static IWICBitmapSourcePtr ApplyEmbeddedICC(IWICBitmapSource* src_bmp, IWICColorContextPtr src_icc, bool restore_from_srgb = false)
    {
        auto   dest_icc = WIC::CreateSystemColorContext_SRGB();
        if (restore_from_srgb)
        {
            std::swap(src_icc, dest_icc);
        }

        // ��Ҫ�ı�icc���ظ�ʽ����һ�ν���.cr2��ʽ��������ʱ��
        auto   format = WIC::GetPixelFormat(src_bmp);
        if (auto trans = WIC::CreateColorTransformer())
        {
            if (trans->Initialize(src_bmp, src_icc, dest_icc, format) == S_OK)
                return trans;
        }
        // assert(false); �ص�assert��һЩgoogle sRGBҲת��ʧ��
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
};

_PHOXO_NAMESPACE_END
