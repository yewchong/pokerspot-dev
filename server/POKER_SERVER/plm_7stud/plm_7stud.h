#ifndef __table_plm_7stud_h__
#define __table_plm_7stud_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Class    : Cplm_7Stud
  Created  : 06/21/2000

  OVERVIEW : 7 Card Stud logic module. This is a subclass
             of the basic Cplm that implements 7 Card Stud
             specific ante and betting structure.

 ****************************************************************************/

#include "includes.h"
#include "plm/plm.h"

class CTable;
class CChips;


class Cplm_7Stud : public Cplm
{
public:
	Cplm_7Stud(CTable*);
  ~Cplm_7Stud();

// Cplm overrides

    //
    // Ante mfunctions
    //
    virtual void doAnte();
    CPlayer* getButtonPlayer(int* refused);
    void sendBlindRequests(CPlayer* player, const CChips& ante);
    Plm_Action getBlindPost(CPlayer*, const CChips& ante);

    // 
    // Betting mfunctions
    //
    virtual void doBettingRound(int cap, const CChips& limit);
    virtual bool isLastBettingRound();

    
    virtual CPlayer* getOddChipWinner(Winners&);
    virtual void doRake();
    virtual void doCleanup();

private:
    int bettingRound_;

private:
    CPlayer* getPlayerWithLowestUpCard();
    CPlayer* getPlayerWithHighestShowingHand();
};


#endif 
