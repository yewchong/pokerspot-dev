#include "ring.h"

Cring::Cring()
{
	player_ = NULL;
	nextNode_ = this;
};

Cring::Cring(CPlayer *player, Cring *nextNode)
{
	player_ = player;
	nextNode_ = nextNode;
};

void Cring::addPlayer(CPlayer *player)
{
	Cring *nextNode;

	nextNode = this;

	if (player_ == NULL)
		player_ = player;
	else
	{
		do {
			nextNode = nextNode->nextNode_;
		} while (nextNode->nextNode_ != this);

		nextNode->nextNode_ = new Cring(player, this);
	}

};

bool Cring::existsPlayer(CPlayer *player)
{
	Cring *node = this;

	do {
		if (node->player_ == player)
			return true;
        node = node->nextNode_;
	} while (node != this);

	return false;
};

int Cring::countPlayers(int state)
{
	Cring *nextNode;
	int playerCount = 0;

	nextNode = this;

	do {
		if (nextNode->player_ != NULL)
			if (nextNode->player_->matchState(state))
				playerCount++;

		nextNode = nextNode->nextNode_;
	} while (nextNode != this);

	return playerCount;
};

void Cring::removePlayer(CPlayer *player)
{
	Cring *nextNode, *freeNode;

	if (DEBUG & DEBUG_ASSERT)
		ASSERT(existsPlayer(player));


	nextNode = this;

	do {
		nextNode = nextNode->nextNode_;
	} while (nextNode->nextNode_->player_ != player);

	freeNode = nextNode->nextNode_;
	nextNode->nextNode_ = nextNode->nextNode_->nextNode_;
	freeNode->wrapNode();

    // XXX freeNode memory leak? 

    // XXX if first player leaves, table's ring ptr
    // still points to the first ring node?

    // XXX infinite loop problem (first player logged
    // out, this never returned)
};

void Cring::wrapNode(void)
{
	this->nextNode_ = this;
};

CPlayer *Cring::getPlayer(int connfd)
{
	Cring *pRing;

	pRing = this;

	if (this->player_->matchConnfd(connfd))
		return this->player_;

    pRing = nextNode_;
	while ((!pRing->player_->matchConnfd(connfd)) && (pRing != this))
	{
		pRing = pRing->nextNode_;
	}

    if (pRing != this)
        return pRing->player_;

	return NULL;
};

CPlayer *Cring::getPlayer(const char *username)
{
	Cring *pRing;

	pRing = this;

	if (this->player_->matchUsername(username))
		return this->player_;

	while ((!pRing->player_->matchUsername(username)) && (pRing != this))
	{
		pRing = pRing->nextNode_;
	}

	return NULL;
};

Cring *Cring::getNextNode(void)
{
    return nextNode_;
};

CPlayer *Cring::getPlayerFromSlot(int slot)
{
	Cring *pRing;

	pRing = this;

	if (this->player_ == NULL)
		return NULL;

	if (this->player_->matchSlot(slot))
		return this->player_;

    pRing = nextNode_;

	while (pRing != this)
	{
        if (pRing->player_->matchSlot(slot))
            break;
		pRing = pRing->nextNode_;
	}

	if (pRing->player_->matchSlot(slot))
		return pRing->player_;
	else
		return NULL;

};


void Cring::copyRing(Cring *source)
{
	Cring *nextNode;

	nextNode = source;

	do {
		addPlayer(nextNode->player_);
		nextNode = nextNode->nextNode_;
	} while (nextNode != source);
};


void Cring::groomRing(int state)
{
	Cring *nextNode;
	
	nextNode = this;

	do {
		if (!nextNode->player_->matchState(state))
			removePlayer(nextNode->player_);

		nextNode = nextNode->nextNode_;
	} while (nextNode != this);

};

void Cring::extractFromRing(Cring *source, int state)
{
	copyRing(source);
	groomRing(state);
};


CRingIterator::CRingIterator(Cring* ring)
    :
    current_(ring),
    first_  (ring)
{}


bool CRingIterator::next(int state)
{
    if (!current_)
        return false;

    do
    {
        current_ = current_->nextNode_;

        if (current_->player_->matchState(state))
        {   // current player satisfies request
            break;
        }
    }
    while (current_ != first_);

    // did we find player with state?    
    return current_ != first_;
}

// Restart this iterator from first player
// matching 'state'.
bool CRingIterator::restart(int state)
{
    if (!current_)
        return false;

    if (!current_->getPlayer())
        return false;

    if (current_->getPlayer()->matchState(state))
    {
        first_ = current_;
        return true;
    }

    bool rc = next(state);
    if (rc)
    {
        first_ = current_;
    }

    return rc;
}
