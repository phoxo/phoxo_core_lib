#pragma once

_PHOXO_BEGIN

inline void Image::ApplyEffectParallel(ImageEffect& effect, IProgressListener* progress)
{
    ParallelTaskExecutor   exec(*this, effect);
    exec.ParallelExecuteTask(progress);
}

inline bool ImageEffect::IsSupported(const Image& img)
{
    return (img.ColorBits() == 32);
}

inline SIZE ImageEffect::GetScanLineCountPerTask(const Image& img)
{
    CSize   ret(0, 200);
    return ret;
}

_PHOXO_NAMESPACE_END
