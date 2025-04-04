#pragma once

namespace WIC
{
    class OrientationTag
    {
    public:
        static WICBitmapTransformOptions Read(IWICBitmapFrameDecode* frame_decode)
        {
            int   tag = 0;
            if (auto reader = GetMetadataQueryReader(frame_decode))
            {
                CComPROPVARIANT   prop;
                if ((reader->GetMetadataByName(OrientationKeyJpeg, &prop) == S_OK) && (prop.vt == VT_UI2))
                    tag = prop.uiVal;
            }
            return ToWICFlipRotate(tag);
        }

        static bool Write(IWICMetadataQueryWriter* writer, int orientation)
        {
            orientation = std::clamp(orientation, 1, 8);

            // 不要用RemoveMetadataByName删除tag，否则只旋转tag时fast encode会失败
            CComPROPVARIANT   prop((USHORT)orientation);
            if (writer && (writer->SetMetadataByName(OrientationKeyJpeg, &prop) == S_OK))
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

        static int Rotate(int orientation, bool clock_wise)
        {
            orientation = std::clamp(orientation, 1, 8);

            int   vec1[] = { 8, 1,6,3,8, 1 };
            int   vec2[] = { 5, 2,7,4,5, 2 };
            for (int i = 1; i <= 4; i++)
            {
                int   pos = i + (clock_wise ? 1 : -1);
                if (vec1[i] == orientation) return vec1[pos];
                if (vec2[i] == orientation) return vec2[pos];
            }
            return 0;
        }

    private:
        static constexpr PCWSTR   OrientationKeyJpeg = L"/app1/ifd/{ushort=274}"; // In a TIFF file, use /ifd/{ushort=274}

        static auto GetMetadataQueryReader(auto* frame_decode)
        {
            IWICMetadataQueryReaderPtr   t;
            if (frame_decode) { frame_decode->GetMetadataQueryReader(&t); }
            return t;
        }
    };
}
