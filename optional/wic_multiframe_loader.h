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
        m_decoder = WIC::CreateDecoderFromFileNoLock(image_path);
        m_format = WIC::GetContainerFormat(m_decoder);
        m_total = WIC::GetFrameCount(m_decoder);
        m_current_frame = 0;
        m_frame = WIC::GetFrame(m_decoder, 0);
        m_use_embedded_icc = use_embedded_icc;
    }

    bool IsWebp() const { return (m_format == GUID_ContainerFormatWebp); }
    bool IsGif() const { return (m_format == GUID_ContainerFormatGif); }
    bool IsJxl() const { return (m_format == WIC::GUID_ContainerFormat_Jxl); }
    bool IsCurrentFrameValid() const { return (m_frame != NULL); }
    UINT GetCurrentFrameIndex() const { return m_current_frame; }

    void SelectNextFrame()
    {
        m_current_frame++;
        if (m_current_frame < m_total)
        {
            m_frame = WIC::GetFrame(m_decoder, m_current_frame);
        }
        else
        {
            m_frame = NULL;
        }
    }

    phoxo::Image LoadCurrentFrame(REFWICPixelFormatGUID desired_format) const
    {
        return phoxo::CodecWIC::LoadFrame(m_frame, desired_format, m_use_embedded_icc);
    }

    int GetDuration() const
    {
        CFindDurationTag   finder(DurationKey());
        int   time = finder.FindDuration(m_frame);
        if (IsGif() || IsJxl())
        {
            time = time * 10;
            if (!time)
                time = 100; // default 10 FPS
        }
        assert(time);
        return time;
    }

private:
    CString DurationKey() const
    {
        if (IsGif()) return L"Delay";
        if (IsWebp()) return L"FrameDuration";
        if (IsJxl()) return L"DurationInTicks";
        return L"";
    }

    class CFindDurationTag : private WIC::MetadataIterator
    {
    private:
        const CComPROPVARIANT   m_meta_key;
        int   m_result = 0;

    public:
        CFindDurationTag(PCWSTR key) : m_meta_key(key)
        {
        }

        int FindDuration(IWICBitmapFrameDecode* frame_decode)
        {
            EnumAllMetadata(frame_decode);
            return m_result;
        }

    private:
        void OnBeforeEnumReader(IWICMetadataReader* reader, REFGUID meta_format) override
        {
            CComPROPVARIANT   val;
            reader->GetValue(NULL, &m_meta_key, &val);
            if (val.vt)
            {
                m_result = val.ParseInteger();
            }
        }
    };
};
