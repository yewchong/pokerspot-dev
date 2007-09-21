#ifndef __lounge_pinger_h__
#define __lounge_pinger_h__

class CPingCounter;

class CPinger
{
public:
    void tick(long now);

private:
    void tickPlayer(CPingCounter* pingCounter, long now);
};

#endif

