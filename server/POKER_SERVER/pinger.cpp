#include "def.h"
#include "sys.h"
#include "pinger.h"
#include "pdupingpong/pduping.h"
#include "player/pingcounter.h"
#include "player/player.h"

void CPinger::tick(long now)
{
    // Do the ping iteration only every fifth tick
    static int i = 0;
    if (++i < 5)
        return;
    i = 0;

    SingleLock l(&mutex_);
    l.lock();

    for (set<CPingCounter*>::iterator it = toPing_.begin(),
                                      end = toPing_.end();
         it != end;
         ++it)
    {
        CPingCounter* pinger = (*it);
        if (!pinger->getPlayer()->getSendError())
            tickPlayer(pinger, now);
    }                                        
}

void CPinger::tickPlayer(CPingCounter* pingCounter, long now)
{
    CPlayer* player = pingCounter->getPlayer();

    // Send a ping after interval
    if (now - pingCounter->getLastPingSent() > PING_DELTA)
    {
        CpduPing pdu;
        if (pdu.sendPing(player) == -1)
        {
            char buf[200];
            sprintf(buf, "Network error(1): player %s",
                    player->getUsername());
            Sys_LogError(buf);

            player->setSendError(true);
        }
        else
        {
            pingCounter->setLastPingSent(now);
        }
    }

    if (!player->getSendError())
    {
        pingCounter->tickPing(now);
    
        if (pingCounter->pingsFailed() > MAX_PING_FAILURES)
        {
            char buf[200];
            sprintf(buf, "Network error(2): player %s",
                    player->getUsername());
            Sys_LogError(buf);

            player->setSendError(true);
        }
    }
}


void CPinger::addPinged(CPlayer* player)
{
    SingleLock l(&mutex_);
    l.lock();

    toPing_.insert(player->getPingCounter());
}

void CPinger::removePinged(CPlayer* player)
{
    SingleLock l(&mutex_);
    l.lock();

    set<CPingCounter*>::iterator pos = 
        toPing_.find(player->getPingCounter());
    if (pos != toPing_.end())
    {
        toPing_.erase(pos);
    }
}
