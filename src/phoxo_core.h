/*
    Copyright (C) =PhoXo= Fu Li

    Author   :  Fu Li
    Create   :
    Home     :  http://www.phoxo.com
    Mail     :  atphoxo@gmail.com

    This file is part of PhoXo

    This code is licensed under the terms of the MIT license.
*/
#pragma once

// core
#include "include/image.h"
#include "include/image_handle.h"
#include "include/image_effect_parallel.h"
#include "include/post_implement.h"

// codec
#include "codec_gdiplus/codec_gdiplus.h"
#include "codec_wic/codec_wic.h"

// effect
#include "effect/basic.h"
#include "effect/composite.h"

#include "effect/stack_blur.h"
#include "effect/unsharp_mask.h"
#include "effect/soft_glow.h"

#include "effect/clipboard.h"
#include "effect/resize_gdiplus.h"
#include "effect/resize_wic.h"

#include "d2d/render_target.h"

_PHOXO_BEGIN
/// Init / Uninit
class CoreLib
{
private:
    static inline ULONG_PTR   m_token = 0;
    static inline HRESULT   m_COM_result = E_INVALIDARG;

public:
    static void Init()
    {
        m_COM_result = ::CoInitialize(0); // init COM

        Gdiplus::GdiplusStartupInput   si;
        Gdiplus::GdiplusStartup(&m_token, &si, NULL); // init GDIPLUS

        WIC::CreateWICFactory(); // init WIC
        WIC::GetSystemCodecFormat(L"");
    }

    static void Uninit()
    {
        WIC::g_factory = nullptr;
        Gdiplus::GdiplusShutdown(m_token);
        if (SUCCEEDED(m_COM_result))
        {
            ::CoUninitialize();
        }
    }
};
_PHOXO_NAMESPACE_END
