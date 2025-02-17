#pragma once
#include "wic_func.h"
#include "wic_metadata_enumerator.h"
#include "wic_metadata_orientation.h"
#include "wic_installed_codec.h"

class CWICMetadata : public CWICMetadataEnumerator
{
public:
    CString   m_date_taken;
    int   m_orientation = 0;
    CString   m_GPS;
    int       m_dpi = 0;
    CString   m_exif_equip_make;
    CString   m_exif_equip_model;
    CString   m_exif_software_used;
    CString   m_exif_lens_model;
    CString   m_exif_F_number;
    CString   m_exif_max_aperture;
    CString   m_exif_exposure_time;
    CString   m_exif_ISO;
    CString   m_exif_focal_length;
    CString   m_exif_metering_mode;
    CString   m_exif_flash;
    CString   m_exif_exposure_program;

public:
    void Read(IWICBitmapFrameDecode* frame_decode)
    {
        if (!frame_decode) { return; }
        m_dpi = CWICFunc::GetResolution(frame_decode);
        EnumAllMetadata(frame_decode);
    }

    bool IsNeedSwapWidthHeight() const
    {
        return (GetRotateFlag() & WICBitmapTransformRotate90);
    }

    WICBitmapTransformOptions GetRotateFlag() const
    {
        return CWICMetadataOrientation::ToWICFlipRotate(m_orientation);
    }

    static void CanonicalizeTakenDate(CString& t)
    {
        if (t.GetLength() == 19)
        {
            PWSTR   p = t.GetBuffer();
            p[4] = p[7] = p[13] = p[16] = ':'; // 有的用T，例如：2017:10:29T11:56:44 or 2017 10 29T11 56 44
            p[10] = ' ';
            t.ReleaseBuffer();
        }
    }

private:
    virtual void OnBeforeEnumReader(IWICMetadataReader* reader, REFCLSID meta_format)
    {
        if (meta_format == GUID_MetadataFormatGps)
        {
            ReadGPS(reader, m_GPS);
        }
    }

    virtual void OnEnumMetadataItem(REFCLSID meta_format, const CComPROPVARIANT& item_id, const CComPROPVARIANT& val)
    {
        if (item_id.vt != VT_UI2)
            return;

        if (meta_format == GUID_MetadataFormatIfd)
        {
            switch (item_id.uiVal)
            {
            case PropertyTagOrientation: m_orientation = val.ParseInteger(); break;
            case PropertyTagEquipMake: m_exif_equip_make = val.ParseString(); break;
            case PropertyTagEquipModel: m_exif_equip_model = val.ParseString(); break;
            case PropertyTagSoftwareUsed: m_exif_software_used = val.ParseString(); break;
            }
        }
        else if (meta_format == GUID_MetadataFormatExif)
        {
            switch (item_id.uiVal)
            {
            case PropertyTagExifDTOrig:
                m_date_taken = val.ParseString();
                break;
            case PropertyTagExifFNumber:
                m_exif_F_number.Format(L"%.1f", val.ParseRational());
                break;
            case PropertyTagExifISOSpeed:
                m_exif_ISO.Format(L"%d", val.ParseInteger());
                break;
            case PropertyTagExifMaxAperture:
                m_exif_max_aperture.Format(L"%.1f", val.ParseRational());
                break;
            case PropertyTagExifFocalLength:
                m_exif_focal_length.Format(L"%.1f", val.ParseRational());
                break;
            case PropertyTagExifMeteringMode:
                m_exif_metering_mode.Format(L"%d", val.ParseInteger());
                break;
            case PropertyTagExifFlash:
                m_exif_flash.Format(L"%d", val.ParseInteger());
                break;
            case PropertyTagExifExposureProg:
                m_exif_exposure_program.Format(L"%d", val.ParseInteger());
                break;
            case 42036:
                m_exif_lens_model = val.ParseString();
                break;
            case PropertyTagExifExposureTime:
                if ((val.vt == VT_UI8) && val.uhVal.LowPart)
                {
                    m_exif_exposure_time.Format(L"%d", (int)(val.uhVal.HighPart / val.uhVal.LowPart));
                }
                break;
            }
        }
    }

    static CString ReadGPSxy(IWICMetadataReader* reader, USHORT k1, USHORT k2, PCWSTR k3)
    {
        CString   ret;
        CComPROPVARIANT   prop;
        if (reader->GetValue(NULL, CComPROPVARIANT(k1), &prop) == S_OK)
        {
            float   loc = prop.ParseGPSLocation();
            prop.Clear();
            if (reader->GetValue(NULL, CComPROPVARIANT(k2), &prop) == S_OK)
            {
                if (prop.ParseString().CompareNoCase(k3) == 0)
                    loc *= -1;
            }
            ret.Format(L"%f", loc);
        }
        return ret;
    }

    static void ReadGPS(IWICMetadataReader* reader, CString& out_data)
    {
        CString   latitude = ReadGPSxy(reader, PropertyTagGpsLatitude, PropertyTagGpsLatitudeRef, L"S");
        if (latitude.GetLength())
        {
            CString   longitude = ReadGPSxy(reader, PropertyTagGpsLongitude, PropertyTagGpsLongitudeRef, L"W");
            if (longitude.GetLength())
            {
                out_data = latitude + L"," + longitude;
            }
        }
    }
};
