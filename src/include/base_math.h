#pragma once

_PHOXO_BEGIN

/// Helper
class Math
{
public:
    static BYTE Clamp0255(int n)
    {
        return (BYTE)std::clamp(n, 0, 0xFF);
    }

    static BYTE Clamp0255(const double& d)
    {
        // if d is very large, Clamp0255((int)d) will result in 0 due to integer overflow, we must first clamp to [0,255] before converting.
        return (BYTE)(std::clamp(d, 0.0, 255.0) + 0.5);
    }

    static int CalcStride(int width, int bpp)
    {
        return 4 * ((width * bpp + 31) / 32);
    }

    template<class T>
    static bool AlmostEqual(const T& a, const T& b)
    {
        return std::fabs(a - b) < std::numeric_limits<T>::epsilon();
    }
};

_PHOXO_NAMESPACE_END
