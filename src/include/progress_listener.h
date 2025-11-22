#pragma once

_PHOXO_BEGIN

/// Interface for receiving updates on image processing progress.
class IProgressListener
{
private:
    bool   m_canceled = false;
    int   m_fix_progress = -1;
//    std::pair<int, int>   m_range;

public:
    virtual ~IProgressListener() = default;

    bool IsUserCanceled() const { return m_canceled; }
    void BeginFixProgress(int fix_progress) { m_fix_progress = fix_progress; }
    void EndFixProgress() { m_fix_progress = -1; }

    bool UpdateProgress(int finish_percent)
    {
        if (m_fix_progress != -1)
            finish_percent = m_fix_progress;

        if (!OnProgressUpdated(finish_percent))
        {
            m_canceled = true;
        }
        return !m_canceled;
    }

protected:
    /// Return false to stop the process, true to continue. \n 0 <= finish_percent <= 100
    virtual bool OnProgressUpdated(int finish_percent) = 0;
};

_PHOXO_NAMESPACE_END
