#ifndef __lounge_pingcounter_h__
#define __lounge_pingcounter_h__

#define MAX_PING_FAILURES 2
#define PING_DELTA        90 // seconds
#define MAX_PING_TIME     15   // seconds

class CPlayer;

class CPingCounter
{
public:
    CPingCounter(CPlayer* p)
        : player_(p), lastPing_(0), activePing_(0), failed_(0)
    {}

    CPlayer* getPlayer() const
    {
        return player_;
    }

    long getLastPingSent() const;
    long getActivePing() const;
    void setLastPingSent(long);
    void tickPing(long);
    int  pingsFailed() const;
    void setPongReceived(long);

private:
    CPlayer* player_;
    long lastPing_, activePing_;
    int  failed_;
};

#endif
