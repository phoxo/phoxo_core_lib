#pragma once

_PHOXO_BEGIN
_PHOXO_EFFECT_BEGIN

/// Iterate all pixels in the region (32 bit).
template<class T>
class PixelIterator : public ImageEffect
{
    void ProcessRegion(Image& img, CRect rc, IProgressListener* progress) override final
    {
        img.IterateRangePixels(rc, (T&)*this, T::HandlePixel, progress);
    }
};

_PHOXO_NAMESPACE_END
_PHOXO_NAMESPACE_END
