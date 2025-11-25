#pragma once

_PHOXO_BEGIN

/// Helper
class Utils
{
public:
    static SIZE GetBitmapSize(HBITMAP bmp)
    {
        BITMAP   bm{};
        ::GetObject(bmp, sizeof(bm), &bm);
        return { bm.bmWidth, bm.bmHeight };
    }

    /// typical : mod = (HMODULE)&__ImageBase
    static IStreamPtr LoadResource(UINT nID, PCWSTR resource_type, HMODULE mod = NULL)
    {
        auto   hres = FindResource(mod, MAKEINTRESOURCE(nID), resource_type);
        auto   ptr = LockResource(::LoadResource(mod, hres));
        return CreateMemStream(ptr, SizeofResource(mod, hres));
    }

    static float CalcFitZoomRatio(SIZE target_area, SIZE image_size)
    {
        assert(image_size.cx && image_size.cy);
        float   dx = (float)target_area.cx / (float)image_size.cx;
        float   dy = (float)target_area.cy / (float)image_size.cy;
        return (std::min)({ dx, dy, 1.0f });
    }

    static CRect CalculateFitWindow(SIZE obj_size, const CRect& wnd_rect)
    {
        int   w = wnd_rect.Width();
        int   h = wnd_rect.Height();
        if ((obj_size.cx > 0) && (obj_size.cy > 0) && (w > 0) && (h > 0))
        {
            if ((obj_size.cx > w) || (obj_size.cy > h))
            {
                double   dx = w / (double)obj_size.cx;
                double   dy = h / (double)obj_size.cy;
                double   d = (std::min)(dx, dy);
                obj_size.cx = (std::max)((int)(obj_size.cx * d), 1);
                obj_size.cy = (std::max)((int)(obj_size.cy * d), 1);
            }

            CRect   rc;
            rc.left = wnd_rect.left + (w - obj_size.cx) / 2;
            rc.top = wnd_rect.top + (h - obj_size.cy) / 2;
            rc.BottomRight() = rc.TopLeft() + obj_size;
            return rc;
        }
        return CRect();
    }

    static ULONG __DEBUG_QueryRefCount(IUnknown* p)
    {
        p->AddRef();
        return p->Release();
    }

    static IStreamPtr CreateMemStream(const void* ptr, UINT mem_size)
    {
        IStreamPtr   t;
        if (ptr) { t.Attach(SHCreateMemStream((const BYTE*)ptr, mem_size)); }
        return t;
    }

    static IStreamPtr CreateStreamFromFileNoLock(PCWSTR filepath)
    {
        IStreamPtr   t;
        SHCreateStreamOnFileEx(filepath, STGM_READ | STGM_SHARE_DENY_NONE, 0, FALSE, NULL, &t); assert(t);
        return t;
    }

    static CString NewGUIDString()
    {
        GUID   g{};
        CoCreateGuid(&g);
        CString   s;
        StringFromGUID2(g, s.GetBuffer(64), 64);
        s.ReleaseBuffer();
        s.Remove(L'{'); s.Remove(L'}'); s.Remove(L'-');
        return s;
    }
};

_PHOXO_NAMESPACE_END
