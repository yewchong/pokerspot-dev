#ifndef __table_pot_h__
#define __table_pot_h__

/*******************************************************************************

    NAME:       class Cpot
    DATE:       4/12/00
    PURPOSE:    Class Cpot defines the pot class, and implements the functions
                needed to interact with this class.  These functions are :
                    - add to pot
                    - subtract from pot
                    - blah
                    - blah
                

*******************************************************************************/


#include "includes.h"
#include "player/player.h"
#include "common/chips.h"


class Cpot
{
public:
  Cpot();

  bool isEligible(CPlayer*);
  void makeEligible(CPlayer*);
  void clearEligible(CPlayer*);
  CPlayer* getEligible(int) const;
  CChips getTotalChips() const;

  bool isComplete() const;
  bool isActive() const;
  void activate();
  void deActivate();
  void complete();
  void unComplete();
  void setStake(const CChips&);
  void setBet(const CChips&);
  void setNext(Cpot*);

  CChips getChips() const;
  CChips getStake() const;
  CChips getBet() const;
  CChips getPlayerChips(CPlayer*) const;

  Cpot* getNext() const;
  void addPlayerChips(CPlayer*, const CChips&);
  void subtractPlayerChips(CPlayer*, const CChips&);
  void zeroChips();
  void subtractChips(const CChips&);

  void killList();
  void clear();
  void setBetComplete(const CChips&);
  CChips getBetComplete() const;

#ifdef DEBUG
  void print();
  void printPlayers();
#endif
  
private:
  enum {
    NumPlayers = 10
  };
  CChips chips_[NumPlayers]; // chips each player has in this pot (indexed by slot)
  CChips bet_;               // current bet in this pot
  CChips stake_;
  CChips betComplete_;       // chips required to complete bet
  bool completed_;
  bool active_;

  CPlayer* eligible_[NumPlayers];
  Cpot* next_;
};


inline void Cpot::setBetComplete(const CChips& b) 
{
  betComplete_ = b;
}

inline CChips Cpot::getBetComplete() const 
{
  return betComplete_;
}


#endif 