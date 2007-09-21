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

#include "pot.h"
#include "common/stringoutput.h"
#include <iostream>


Cpot::Cpot()
  :
  completed_(true),
  active_(true),
  next_(NULL)
{
  memset(eligible_, 0x0, sizeof(eligible_));
}

CChips Cpot::getChips() const
{
  CChips totalChips;

  for (int i = 0; i < NumPlayers; i++)
  {
    totalChips += chips_[i];
  }
    
  return totalChips;
}

// Return sum of chips in this and pots in
// list after this pot.
CChips Cpot::getTotalChips() const
{
  if (getNext())
    return getChips() + getNext()->getTotalChips();
  else
    return getChips();
}

CChips Cpot::getStake() const
{
  return stake_;
}

CChips Cpot::getBet() const
{
  return bet_;
}

void Cpot::setBet(const CChips& bet)
{
  bet_ = bet;
}

void Cpot::setNext(Cpot* next)
{
  next_ = next;
}

bool Cpot::isActive() const
{
  return active_;
}

bool Cpot::isComplete() const
{
  return completed_;
}

void Cpot::activate() 
{
  active_ = true;
}

void Cpot::deActivate()
{
  active_ = false;
}

void Cpot::complete()
{
  completed_ = true;
}

void Cpot::unComplete()
{
  completed_ = false;
}

void Cpot::setStake(const CChips& stake)
{
  stake_ = stake;
}

void Cpot::zeroChips()
{
  int i;

  for (i = 0; i < 10; i++)
  {
    chips_[i] = 0;
  }
  return;
};

void Cpot::subtractChips(const CChips& c)
{ // This function subtracts chips from the pot evenly across all players
  int i = 0;
  CChips numChips = getChips();
  CChips chips(c);
  CChips oneChip(1, 0);

  while (chips > 0 && numChips >= chips)
  {
    for (i = 0; (chips > 0) && (i < 10); i++)
    {
      if (chips_[i] > 0)
      {
        chips_[i] -= oneChip;
        chips -= oneChip;
      }
    }
  }    
}

void Cpot::addPlayerChips(CPlayer* player, const CChips& chips)
{
  chips_[player->getSlot()] += chips;
}

void Cpot::subtractPlayerChips(CPlayer* player, const CChips& chips)
{
  ASSERT(chips_[player->getSlot()] >= chips);
  chips_[player->getSlot()] -= chips;
}

CChips Cpot::getPlayerChips(CPlayer* player) const
{
  return chips_[player->getSlot()];
};

Cpot* Cpot::getNext() const
{
  return next_;
};

bool Cpot::isEligible(CPlayer* player)
{   // This function returns true if player is eligible for this pot

    if (!player) return false;

    if (eligible_[player->getSlot()] == player)
        return true;
    else
        return false;
};

CPlayer* Cpot::getEligible(int i) const
{   
    return eligible_[i];
};


void Cpot::makeEligible(CPlayer *player)
{
    // first, make sure the slot in eligible_ isn't taken
    if (eligible_[player->getSlot()] == NULL)
    {
        eligible_[player->getSlot()] = player;
        return;
    }
    else
    {
        if (DEBUG & DEBUG_TABLE)
            printf("Unable to make player eligible for this pot!\n");

        return;
    }
};

void Cpot::clearEligible(CPlayer *player)
{
    if(!isEligible(player))
    {
        if (DEBUG & DEBUG_TABLE)
        {
            printf("Player is not currently even eligible!\n");
        }

        return;
    }

    if (eligible_[player->getSlot()] == player)
    {
        eligible_[player->getSlot()] = NULL;
    }
};

void Cpot::killList()
{
    memset(&eligible_[0], 0x0, sizeof(eligible_));
};

void Cpot::clear()
{
    next_ = NULL;
    completed_ = true;
    active_ = true;
    bet_ = 0;
    betComplete_ = 0; // XXX
    stake_ = 0;
    for (int i = 0; i < 10; i++)
    {
        chips_[i] = 0;
    }

    killList();
}

#ifdef DEBUG
void Cpot::printPlayers()
{
#ifdef DEBUG_POT_
    int i;

    for (i = 0; i < 10; i++)
    {
        if (eligible_[i] != NULL)
        {
            printf("%s has %f chips in the pot.\n",
                   eligible_[i]->getUsername(),
                   chips_[i].asDouble());
        }
    }
#endif
};

void Cpot::print()
{  
#ifdef DEBUG_POT_

  std::cout << "Chips=" << getChips()
            << " Stake=" << getStake()
            << " Bet=" << getBet() << endl;

  cout << "   chips ---> [";
  for (int i = 0; i < 9; i++)
  {
    cout << chips_[i] << ' ';
  }
  cout << chips_[9] << ']' << endl;

#endif
};  

#endif