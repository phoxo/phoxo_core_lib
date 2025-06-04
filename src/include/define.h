#pragma once

/// @cond
#include <windows.h>
#include <comdef.h> // for SDK app, must include this before include GDI+
#include <shlwapi.h>
#pragma comment (lib, "Shlwapi.lib")

#include <cmath>
#include <cassert>
#include <memory>
#include <string>
#include <vector>
#include <deque>
#include <algorithm>

#ifndef GDIPVER
    #define GDIPVER 0x0110
#endif
#include <GdiPlus.h>
#pragma comment (lib, "GdiPlus.lib")
using GPointF = Gdiplus::PointF;
using GRectF = Gdiplus::RectF;
/// @endcond

//-------------------------------------------------------------------------------------
#define _PHOXO_NAMESPACE(name) namespace name {
#define _PHOXO_BEGIN  namespace phoxo {
#define _PHOXO_INTERNAL_BEGIN  namespace internal {
#define _PHOXO_EFFECT_BEGIN  namespace effect {
#define _PHOXO_NAMESPACE_END   }

_PHOXO_BEGIN
/// The alias for RGBQUAD, rgbReserved is too ugly.
union RGBA32bit
{
    RGBQUAD   quad;
    struct { BYTE b, g, r, a; };

    operator RGBQUAD() const { return quad; }
    void operator=(const RGBQUAD& c) { quad = c; }

    void PremulSum(double& sb, double& sg, double& sr, double& sa, double coef) const
    {
        //  sb += (b * a) * coef;
        //  sg += (g * a) * coef;
        //  sr += (r * a) * coef;
        //  sa += a * coef;

        // The following code is slightly faster
        double   ac = a * coef;
        sb += b * ac;
        sg += g * ac;
        sr += r * ac;
        sa += ac;
    }
};
_PHOXO_NAMESPACE_END

//-------------------------------------------------------------------------------------
#include "wic/wic_interface.h"
#include "wic/wic_interface2.h"
#include "wic/wic_interface3.h"
#include "wic/wic_bitmap_lock.h"
#include "wic/wic_orientation_tag.h"
#include "wic/wic_metadata_iterator.h"
#include "wic/wic_metadata.h"
#include "wic/wic_factory.h" // 这两个文件依赖全局 g_factory
#include "wic/wic_system_codec.h"

#include "base_utils.h"
#include "base_bitmap_hdc.h"
#include "base_math.h"
#include "base_file_ext.h"
#include "progress_listener.h"
#include "color.h"
#include "pixel_func.h"
