#pragma once

namespace WIC
{
    class BitmapLock
    {
    private:
        IWICBitmapLockPtr   m_lock;

    public:
        UINT   m_width = 0;
        UINT   m_height = 0;
        UINT   m_stride = 0;
        UINT   m_buf_size = 0;
        BYTE   * m_data = nullptr;
        WICPixelFormatGUID   m_format = {};

    public:
        BitmapLock(IWICBitmapPtr src)
        {
            try
            {
                src->GetSize(&m_width, &m_height);
                WICRect   rc = { 0, 0, (INT)m_width, (INT)m_height };
                src->Lock(&rc, WICBitmapLockRead | WICBitmapLockWrite, &m_lock);
                m_lock->GetStride(&m_stride);
                m_lock->GetDataPointer(&m_buf_size, &m_data);
                src->GetPixelFormat(&m_format);
            }
            catch (_com_error&) {}
            assert(m_data);
        }

        // 很多开源库输出是 32bppRGBA
        bool IsFormat32bpp() const
        {
            return m_data && (m_width * 4 == m_stride);
        }
    };
}
