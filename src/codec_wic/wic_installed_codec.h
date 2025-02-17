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
                m_format_ext = L"," + m_format_ext + L",";
            }
        }
    };

private:
    static inline std::vector<CodecInfo>   g_decoders_info;

public:
    static void Init()
    {
        FindContainerFormat(L"");
    }

    static CLSID FindContainerFormat(PCWSTR file_path)
    {
        if (!g_decoders_info.size())
        {
            FindAllDecoder();
        }

        CString   ext = PathFindExtension(file_path);
        if (!ext.IsEmpty())
        {
            ext = L"," + ext + L",";
            for (auto& iter : g_decoders_info)
            {
                if (StrStrI(iter.m_format_ext, ext))
                    return iter.m_container_format;
            }
        }
        return CLSID_NULL;
    }

private:
    static void FindAllDecoder()
    {
        IEnumUnknownPtr   root;
        CWICFunc::g_factory->CreateComponentEnumerator(WICDecoder, WICComponentEnumerateDefault, &root);
        if (!root)
            return;

        IUnknownPtr   unkp;
        while (root->Next(1, &unkp, NULL) == S_OK)
        {
            IWICBitmapCodecInfoPtr   ifp = unkp;
            if (ifp)
            {
                CodecInfo   info(*ifp);
                g_decoders_info.push_back(info);
            }
            unkp = NULL;
        }

        g_decoders_info.shrink_to_fit();
    }
};
