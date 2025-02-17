#pragma once

class CWICMultiframeLoader
{
private:
    IWICBitmapDecoderPtr   m_decoder;
    GUID   m_format;
    UINT   m_total;
    UINT   m_current_frame;
    IWICBitmapFrameDecodePtr   m_frame;
    bool   m_use_embedded_icc;

public:
    CWICMultiframeLoader(PCWSTR image_path, bool use_embedded_icc = true)
    {
        m_decoder = CWICFunc::CreateDecoderFromFileNoLock(image_path);
        m_format = CWICFunc::GetContainerFormat(m_decoder);
        m_total = CWICFunc::GetFrameCount(m_decoder);
        m_current_frame = 0;
        m_frame = CWICFunc::GetFrame(m_decoder, 0);
        m_use_embedded_icc = use_embedded_icc;
    }

    bool IsWebp() const { return (m_format == GUID_ContainerFormatWebp); }
    bool IsGif() const { return (m_format == GUID_ContainerFormatGif); }
    bool IsCurrentFrameValid() const { return (m_frame != NULL); }
    UINT GetCurrentFrameIndex() const { return m_current_frame; }

    void SelectNextFrame()
    {
        m_current_frame++;
        if (m_current_frame < m_total)
        {
            m_frame = CWICFunc::GetFrame(m_decoder, m_current_frame);
        }
        else
        {
            m_frame = NULL;
        }
    }

    void LoadCurrentFrame(phoxo::Image& out_image, REFWICPixelFormatGUID desired_format) const
    {
        phoxo::CodecWIC::LoadFrame(m_frame, out_image, desired_format, m_use_embedded_icc);
    }

    int GetDuration() const
    {
        CFindDurationTag   finder;
        int   time = finder.FindDuration(m_frame, IsGif());
        if (IsGif())
        {
            time = time * 10;
            if (!time)
                time = 100; // default 10 FPS
        }
        assert(time);
        return time;
    }

private:
    class CFindDurationTag : private CWICMetadataEnumerator
    {
    private:
        CComPROPVARIANT   m_meta_key;
        int   m_result = 0;

    public:
        int FindDuration(IWICBitmapFrameDecode* frame_decode, bool is_gif)
        {
            m_meta_key.Set(is_gif ? L"Delay" : L"FrameDuration");
            EnumAllMetadata(frame_decode);
            return m_result;
        }

    private:
        virtual void OnBeforeEnumReader(IWICMetadataReader* reader, REFCLSID meta_format)
        {
            CComPROPVARIANT   val;
            reader->GetValue(NULL, &m_meta_key, &val);
            if (val.vt)
            {
                m_result = val.ParseInteger();
            }
        }
    };

public:
    static UINT GetIconMaxSizeFrameIndex(IWICBitmapDecoder* decoder)
    {
        UINT   total = CWICFunc::GetFrameCount(decoder);
        UINT   index = 0;
        int   max_size = 0;
        for (UINT i = 0; i < total; i++)
        {
            auto   frame = CWICFunc::GetFrame(decoder, i);
            int   frame_size = CWICFunc::GetBitmapSize(frame).cx;
            if (frame_size > max_size)
            {
                max_size = frame_size;
                index = i;
            }
        }
        return index;
    }
};
