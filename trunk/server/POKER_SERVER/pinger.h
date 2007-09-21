#ifndef __table_pinger_h__
#define __table_pinger_h__

#include <set>
#include "sync/sync.h"
#include "compat.h"
USING_NS_T(std, set);


class CPingCounter;
class CPlayer;

class CPinger
{
public:
    void tick(long now);

    void addPinged(CPlayer*);
    void removePinged(CPlayer*);

private:
    void tickPlayer(CPingCounter* pingCounter, long now);

    Mutex mutex_;
    set<CPingCounter*> toPing_;
};

#endif

