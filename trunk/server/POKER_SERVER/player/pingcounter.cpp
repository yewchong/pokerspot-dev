#include "pingcounter.h"


void CPingCounter::tickPing(long now)
{
    if (activePing_ > 0)
    {
        if (now - activePing_ > MAX_PING_TIME)
        {
            activePing_ = 0;
            failed_++;
        }
    }
}

void CPingCounter::setLastPingSent(long now)
{
    lastPing_ = now;
    activePing_ = now;
}


long CPingCounter::getLastPingSent() const
{
    return lastPing_;
}

int CPingCounter::pingsFailed() const
{
    return failed_;
}

void CPingCounter::setPongReceived(long now)
{
    if (activePing_ > 0)
    {
        if (now - activePing_ > MAX_PING_TIME)
        {
            failed_++;
        }
        else
        {
            // Successfull ping clears previous failures
            failed_ = 0;
        }
        activePing_ = 0;
    }
}
