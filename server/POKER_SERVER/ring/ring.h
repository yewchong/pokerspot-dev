#ifndef __ring_h__
#define __ring_h__

#include "../player/player.h"

class Cring
{
public:
	Cring();
	Cring(CPlayer *, Cring *);
	CPlayer *getPlayer(const char *);
	CPlayer *getPlayer(int);
	CPlayer *getPlayerFromSlot(int);
    CPlayer *getPlayer() const;
    Cring *getNextNode(void);
	void addPlayer(CPlayer *);
	void removePlayer(CPlayer *);
	void copyRing(Cring *);
	void extractFromRing(Cring *, int);
	void groomRing(int);
	int countPlayers(int);
	void wrapNode(void);
	bool existsPlayer(CPlayer *);
    void zeroPlayer();
private:
	CPlayer	*player_;
	Cring *nextNode_;

    friend struct CRingIterator;
};

inline CPlayer *Cring::getPlayer() const
{
    return player_;
}

inline void Cring::zeroPlayer()
{
    player_ = NULL;
}


struct CRingIterator
{
    CRingIterator(Cring*);
    // advance to next player
    bool operator++();
    // advance to next player with state
    bool next(int state);

    Cring* current() const;
    bool   start(int);
    void   restart(Cring*);
    bool   restart(int state);

private:
    Cring* current_;
    Cring* first_;
};

inline bool CRingIterator::operator++()
{
    if (!current_)
        return false;

    current_ = current_->nextNode_;
    return current_ != first_;
}

inline Cring* CRingIterator::current() const
{
    return current_;
}

inline bool CRingIterator::start(int state)
{
    if (!current_)
        return false;

    if (!current_->getPlayer())
        return false;

    if (current_->getPlayer()->matchState(state))
        return true;

    return next(state);

}

inline void CRingIterator::restart(Cring* r)
{
    first_ = r;
}


#endif