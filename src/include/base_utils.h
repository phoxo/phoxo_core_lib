#pragma once
/// @cond
#include <shlwapi.h>
#pragma comment (lib, "Shlwapi.lib")
/// @endcond

_PHOXO_BEGIN

/// Helper
class Utils
{
public:
    static CSize GetBitmapSize(HBITMAP bmp)
    {
        BITMAP   bm = {};
        ::GetObject(bmp, sizeof(bm), &bm);
        return CSize(bm.bmWidth, bm.bmHeight);
    }

    /// typical : mod = (HMODULE)&__ImageBase
    static IStreamPtr LoadResource(UINT nID, PCWSTR resource_type, HMODULE mod = nullptr)
    {
        auto   hres = FindResource(mod, MAKEINTRESOURCE(nID), resource_type);
        auto   ptr = LockResource(::LoadResource(mod, hres));
        return CreateMemStream(ptr, SizeofResource(mod, hres));
    }

    static CRect CalculateFitWindow(SIZE obj_size, const CRect& wnd_rect)
    {
        int   w = wnd_rect.Width();
        int   h = wnd_rect.Height();
        if (obj_size.cx && obj_size.cy && (w > 0) && (h > 0))
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
        IStreamPtr   v;
        if (ptr) { v.Attach(SHCreateMemStream((const BYTE*)ptr, mem_size)); }
        return v;
    }
};

_PHOXO_NAMESPACE_END
