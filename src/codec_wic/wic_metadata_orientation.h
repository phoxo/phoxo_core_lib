#pragma once

class CWICMetadataOrientation
{
public:
    static WICBitmapTransformOptions Read(IWICBitmapFrameDecode* frame_decode)
    {
        if (frame_decode)
        {
            IWICMetadataQueryReaderPtr   reader;
            frame_decode->GetMetadataQueryReader(&reader);
            if (reader)
            {
                CComPROPVARIANT   prop;
                if (reader->GetMetadataByName(OrientationKeyJpeg(), &prop) == S_OK)
                {
                    if (prop.vt == VT_UI2)
                    {
                        return ToWICFlipRotate(prop.uiVal);
                    }
                }
            }
        }
        return WICBitmapTransformRotate0;
    }

    static bool Write(IWICMetadataQueryWriter* writer, int orientation)
    {
        orientation = std::clamp(orientation, 1, 8);

        // 不要用RemoveMetadataByName删除tag，否则只旋转tag时fast encode会失败
        CComPROPVARIANT   prop((USHORT)orientation);
        if (writer && (writer->SetMetadataByName(OrientationKeyJpeg(), &prop) == S_OK))
            return true;

        assert(false);
        return false;
    }

    static WICBitmapTransformOptions ToWICFlipRotate(int orientation)
    {
        switch (orientation)
        {
        case 1: break;
        case 2: return WICBitmapTransformFlipHorizontal;
        case 3: return WICBitmapTransformRotate180;
        case 4: return WICBitmapTransformFlipVertical;
        case 5: return (WICBitmapTransformOptions)(WICBitmapTransformRotate90 | WICBitmapTransformFlipVertical);
        case 6: return WICBitmapTransformRotate90;
        case 7: return (WICBitmapTransformOptions)(WICBitmapTransformRotate90 | WICBitmapTransformFlipHorizontal);
        case 8: return WICBitmapTransformRotate270;
        }
        return WICBitmapTransformRotate0;
    }

private:
    static CString OrientationKeyJpeg() { return L"/app1/ifd/{ushort=274}"; } // In a TIFF file, use /ifd/{ushort=274}
};
