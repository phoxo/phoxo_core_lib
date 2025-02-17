#pragma once

/// Bilinear distort (32 bit).
class FCEffectBilinearDistort : public FCImageEffect
{
private:
    FCImage   m_bak;

protected:
    /// check bpp == 32 and width & height >= 2
    virtual bool IsSupport(const FCImage& img)
    {
        return (img.ColorBits() == 32) && !img.IsPremultiplied() && (img.Width() >= 2) && (img.Height() >= 2);
    }

    /// derived class must call OnBeforeProcess of base first.
    virtual void OnBeforeProcess(FCImage& img)
    {
        m_bak = img;
    }

    const FCImage& GetBackupImage() const { return m_bak; }

    /// map current point (x,y) to point (un_x,un_y) in original image.
    virtual void calc_undistorted_coord(int x, int y, double& un_x, double& un_y) = 0;

private:
    /*
        Calculate bilinear interpolation
        0 <= x,y < 1, distance to neighbor[0,0]
        neighbor - in order [0,0], [1,0], [0,1], [1,1].
    */
    static void CalcBilinear(double x, double y, BYTE* neighbor[4], RGBA32bit& output)
    {
        auto   px0 = neighbor[0], px1 = neighbor[1], px2 = neighbor[2], px3 = neighbor[3];
        int   a0 = px0[3], a1 = px1[3], a2 = px2[3], a3 = px3[3];

        if ((a0 & a1 & a2 & a3) == 0xFF)
        {
            // all alpha of pixel is 0xFF
            for (int i = 0; i < 3; i++)
            {
                auto   m0 = x * px1[i] + (1 - x) * px0[i];
                auto   m1 = x * px3[i] + (1 - x) * px2[i];
                auto   my = y * m1 + (1 - y) * m0;
                ((BYTE*)&output)[i] = phoxo::Math::Clamp0255(my);
            }
            output.a = 0xFF;
        }
        else
        {
            auto   m0 = x * a1 + (1 - x) * a0;
            auto   m1 = x * a3 + (1 - x) * a2;
            auto   my = y * m1 + (1 - y) * m0;
            auto   alpha = my;
            output.a = phoxo::Math::Clamp0255(alpha);
            if (output.a)
            {
                for (int i = 0; i < 3; i++)
                {
                    m0 = x * a1 * px1[i] + (1 - x) * a0 * px0[i];
                    m1 = x * a3 * px3[i] + (1 - x) * a2 * px2[i];
                    my = y * m1 + (1 - y) * m0;
                    ((BYTE*)&output)[i] = phoxo::Math::Clamp0255(my / alpha);
                }
            }
        }
    }

    virtual void ProcessPixel(FCImage& img, int x, int y, RGBA32bit* pixel)
    {
        double   un_x = 0, un_y = 0;
        calc_undistorted_coord(x, y, un_x, un_y);

        RGBA32bit   null_clr = { 0,0,0,0 };

        if ((un_x > -1) && (un_x < m_bak.Width()) &&
            (un_y > -1) && (un_y < m_bak.Height()))
        {
            int   srcX = ((un_x < 0) ? -1 : (int)un_x),
                  srcY = ((un_y < 0) ? -1 : (int)un_y),
                  srcX_1 = srcX + 1,
                  srcY_1 = srcY + 1;
            BYTE   * neighbor[4] =
            {
                m_bak.IsInside(srcX,srcY) ? m_bak.GetPixel(srcX,srcY) : (BYTE*)&null_clr,
                m_bak.IsInside(srcX_1,srcY) ? m_bak.GetPixel(srcX_1,srcY) : (BYTE*)&null_clr,
                m_bak.IsInside(srcX,srcY_1) ? m_bak.GetPixel(srcX,srcY_1) : (BYTE*)&null_clr,
                m_bak.IsInside(srcX_1,srcY_1) ? m_bak.GetPixel(srcX_1,srcY_1) : (BYTE*)&null_clr,
            };
            CalcBilinear(un_x - srcX, un_y - srcY, neighbor, *pixel);
        }
        else
        {
            *pixel = null_clr;
        }
    }
};
