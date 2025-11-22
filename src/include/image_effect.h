#pragma once

_PHOXO_BEGIN

// forward declaration
class Image;

/// Interface for image effects.
class ImageEffect
{
private:
    bool   m_parallel = false;

public:
    /// Image processing modes.
    enum class ProcessMode
    {
        SliceBlock, /**< Automatically slice and process when handling in parallel */
        EntireMyself, /**< Custom process the entire image by themselves */
    };

public:
    virtual ~ImageEffect() = default;

    /// @name Parallel processing control.
    ///@{
    bool IsParallelEnabled() const { return m_parallel; }
    void EnableParallel(bool enable) { m_parallel = enable; }
    virtual SIZE GetScanLineCountPerTask(const Image& img) { return { 0, 200 }; }
    ///@}

    /// defaults to check if bpp == 32
    virtual bool IsSupported(const Image& img);
    /// return ProcessMode::SliceBlock by default.
    virtual ProcessMode QueryProcessMode() { return ProcessMode::SliceBlock; }

    /// event before process, by default, does nothing.
    virtual void OnBeforeProcess(Image& img) {}
    virtual void OnAfterProcess(Image& img) {}

    /// process a block of the image when \ref QueryProcessMode return ProcessMode::SliceBlock.
    virtual void ProcessRegion(Image& img, CRect region_rect, IProgressListener* progress) { assert(false); }
    /// process the entire image when \ref QueryProcessMode return ProcessMode::EntireMyself.
    virtual void ProcessEntire(Image& img, IProgressListener* progress) { assert(false); }
};

_PHOXO_NAMESPACE_END
