#pragma once

/// @cond
#include <windows.h>
#include <comdef.h> // for SDK app, must include this before include GDI+

#include <atltypes.h>

#include <cmath>
#include <cassert>
#include <memory>
#include <string>
#include <vector>
#include <deque>
#include <algorithm>
#include <GdiPlus.h>
#pragma comment (lib, "GdiPlus.lib")
/// @endcond

//-------------------------------------------------------------------------------------
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

    template<class T>
    void PremulSum(T& sb, T& sg, T& sr, T& sa, int coef) const
    {
        sb += (b * a) * coef;
        sg += (g * a) * coef;
        sr += (r * a) * coef;
        sa += a * coef;
    }
};
_PHOXO_NAMESPACE_END

//-------------------------------------------------------------------------------------
#include "base_utils.h"
#include "base_bitmap_hdc.h"
#include "base_math.h"
#include "base_file_ext.h"
#include "progress_listener.h"
#include "color.h"
