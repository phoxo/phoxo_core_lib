#pragma once
/// @cond
#include <propvarutil.h>
#pragma comment (lib, "Propsys.lib")
#include <wincodec.h>
#include <Wincodecsdk.h>
#pragma comment (lib, "Windowscodecs.lib")

#define  WICNormal32bpp  GUID_WICPixelFormat32bppBGRA
#define  WICPremultiplied32bpp  GUID_WICPixelFormat32bppPBGRA

_COM_SMARTPTR_TYPEDEF(IWICImagingFactory, __uuidof(IWICImagingFactory));
_COM_SMARTPTR_TYPEDEF(IWICBitmapDecoder, __uuidof(IWICBitmapDecoder));
_COM_SMARTPTR_TYPEDEF(IWICBitmapFrameDecode, __uuidof(IWICBitmapFrameDecode));
_COM_SMARTPTR_TYPEDEF(IWICColorContext, __uuidof(IWICColorContext));
_COM_SMARTPTR_TYPEDEF(IWICBitmapScaler, __uuidof(IWICBitmapScaler));
_COM_SMARTPTR_TYPEDEF(IWICBitmapSource, __uuidof(IWICBitmapSource));
_COM_SMARTPTR_TYPEDEF(IWICBitmap, __uuidof(IWICBitmap));
_COM_SMARTPTR_TYPEDEF(IWICMetadataQueryReader, __uuidof(IWICMetadataQueryReader));
_COM_SMARTPTR_TYPEDEF(IWICBitmapFlipRotator, __uuidof(IWICBitmapFlipRotator));
_COM_SMARTPTR_TYPEDEF(IWICColorTransform, __uuidof(IWICColorTransform));
_COM_SMARTPTR_TYPEDEF(IWICFastMetadataEncoder, __uuidof(IWICFastMetadataEncoder));
_COM_SMARTPTR_TYPEDEF(IWICMetadataQueryWriter, __uuidof(IWICMetadataQueryWriter));
_COM_SMARTPTR_TYPEDEF(IWICBitmapFrameEncode, __uuidof(IWICBitmapFrameEncode));
_COM_SMARTPTR_TYPEDEF(IWICBitmapLock, __uuidof(IWICBitmapLock));
_COM_SMARTPTR_TYPEDEF(IWICBitmapEncoder, __uuidof(IWICBitmapEncoder));
_COM_SMARTPTR_TYPEDEF(IWICStream, __uuidof(IWICStream));
_COM_SMARTPTR_TYPEDEF(IWICMetadataBlockWriter, __uuidof(IWICMetadataBlockWriter));
_COM_SMARTPTR_TYPEDEF(IWICMetadataBlockReader, __uuidof(IWICMetadataBlockReader));
_COM_SMARTPTR_TYPEDEF(IWICPixelFormatInfo, __uuidof(IWICPixelFormatInfo));
_COM_SMARTPTR_TYPEDEF(IWICComponentInfo, __uuidof(IWICComponentInfo));
_COM_SMARTPTR_TYPEDEF(IWICBitmapCodecInfo, __uuidof(IWICBitmapCodecInfo));
_COM_SMARTPTR_TYPEDEF(IWICMetadataReader, __uuidof(IWICMetadataReader));

class CComPROPVARIANT : public PROPVARIANT
{
public:
    CComPROPVARIANT() { PropVariantInit(this); }
    ~CComPROPVARIANT() { Clear(); }

    CComPROPVARIANT(USHORT v) { InitPropVariantFromUInt16(v, this); }
    CComPROPVARIANT(PCWSTR v) { InitPropVariantFromString(v, this); }

    operator LPPROPVARIANT() { return this; }

    void Clear()
    {
        PropVariantClear(this);
    }

    int ParseInteger() const
    {
        if (IsVarTypeInteger(vt))
        {
            return PropVariantToInt32WithDefault(*this, 0);
        }
        assert(false);
        return 0;
    }

    CString ParseString() const
    {
        if (vt == VT_LPSTR)  return CString(pszVal);
        if (vt == VT_LPWSTR)  return CString(pwszVal);

        if ((vt == (VT_LPSTR | VT_VECTOR)) && calpstr.cElems)
            return CString(calpstr.pElems[0]);

        assert(false);
        return L"";
    }

    float ParseRational() const
    {
        if (vt == VT_UI8 || vt == VT_I8)
        {
            if (uhVal.HighPart)
                return uhVal.LowPart / (float)uhVal.HighPart;
            else
                return 0; // 一些手机max aperture值是0，防止反复弹出assert
        }
        assert(false);
        return 0;
    }

    float ParseGPSLocation() const
    {
        if (cauh.cElems >= 3)
        {
            int   t1 = (VT_UI8 | VT_VECTOR);
            int   t2 = (VT_I8 | VT_VECTOR);
            if ((vt == t1) || (vt == t2))
            {
                ULONGLONG   buf[3] = { cauh.pElems[0].QuadPart, cauh.pElems[1].QuadPart, cauh.pElems[2].QuadPart };
                auto   ptr = (ULONG*)buf;
                if (!ptr[1] || !ptr[3] || !ptr[5]) { assert(false); return 0; }

                float   d = ptr[0] / (float)ptr[1];
                float   m = ptr[2] / (float)ptr[3];
                float   s = ptr[4] / (float)ptr[5];
                return d + m / 60 + s / 3600;
            }
        }
        assert(false);
        return 0;
    }
};
/// @endcond
