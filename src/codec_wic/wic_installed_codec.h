#pragma once

class CWICInstalledCodec
{
private:
    struct CodecInfo
    {
        CString   m_format_ext; // such as : ",.jpeg,.jpe,.jpg,.jfif,.exif,"  a comma-wrapped
        CLSID   m_container_format; // such as : GUID_ContainerFormatJpeg

        CodecInfo(IWICBitmapCodecInfo& r)
        {
            r.GetContainerFormat(&m_container_format);

            UINT   read = 0;
            r.GetFileExtensions(0, NULL, &read);
            if (read)
            {
                r.GetFileExtensions(read, m_format_ext.GetBuffer(read), &read);
                m_format_ext.ReleaseBuffer();
                m_format_ext.MakeLower();
                m_format_ext = L"," + m_format_ext + L",";
            }
        }
    };

    struct AllDecoderList : public std::vector<CodecInfo>
    {
        AllDecoderList()
        {
            IEnumUnknownPtr   root;
            CWICFunc::g_factory->CreateComponentEnumerator(WICDecoder, WICComponentEnumerateDefault, &root);
            if (!root)
                return;

            IUnknownPtr   unk;
            while (root->Next(1, &unk, NULL) == S_OK)
            {
                if (IWICBitmapCodecInfoPtr ifp = unk)
                {
                    emplace_back(*ifp);
                }
                unk = nullptr;
            }

            shrink_to_fit();
        }
    };

public:
    static CLSID FindContainerFormat(PCWSTR filepath)
    {
        static const AllDecoderList   s_decoders;

        CString   ext = PathFindExtension(filepath);
        if (!ext.IsEmpty())
        {
            ext.MakeLower();
            ext = L"," + ext + L",";
            for (auto& iter : s_decoders)
            {
                if (wcsstr(iter.m_format_ext, ext))
                    return iter.m_container_format;
            }
        }
        return CLSID_NULL;
    }
};
