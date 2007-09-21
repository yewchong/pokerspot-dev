#ifndef __tem_community_h__
#define __tem_community_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class Community
  Created  : 25.10.1999

  OVERVIEW : Implements the Community player for Texas Hold'em.
             The community maintains the community cards and also
             maintains the pot (and side-pots).

 ****************************************************************************/


#include "base/player.h"
#include "ui/tableview.h"
#include <list>
#include <vector>

// Defining this causes debug messages to
// be printed in the chat window
//#define DEBUG_POT_ 1

struct SidePot
{ 
  CChips bet_;                           // current bet
  CChips chips_[CTableView::MaxPlayers]; // chips in this pot
  BOOL isLivePot_;// each round has a live pot (remaining players eligible)
  BOOL isActive_; // is this pot active?

  BOOL isComplete_;  // is bet completed?
  CChips betComplete_; // chips required to complete

  // Area covered by this pot on the table surface
  CRect area_;
  // Slot indices of players eligible to this pot
  std::vector<int> eligible_players_;

  SidePot();
  SidePot(const CChips& bet);
  SidePot(const SidePot&);
  SidePot& operator = (const SidePot&);
  void clearEligible();
  void addEligible(int slot);

  BOOL isEligible(int slot) const
  {
    return eligible_players_[slot] != FALSE;
  }

  void reset()
  {
    bet_ = 0;
    isActive_ = TRUE;
    isLivePot_ = TRUE;
    isComplete_ = TRUE;
    betComplete_ = 0;
    clearEligible();
    clear_chips();
  }

  CChips get_chips() const
  {
    CChips sum = 0;
    for (int i = 0; i < CTableView::MaxPlayers; i++)
      sum += chips_[i];
    return sum;
  }

  void clear_chips()
  {
    for (int i = 0; i < CTableView::MaxPlayers; i++)
      chips_[i] = 0;
  }

  void add_chips(int player_slot, const CChips& chips)
  {
    chips_[player_slot] += chips;
  }

#ifdef DEBUG_POT_
  void dump(int pot_num);
#endif
};


class Community : public Base::Player
{
public:
  Community(PlayerSlot&);

  // Player overrides
  BOOL    drawImage(CDC* pDC, const CPoint& origo, DWORD flags) const;
  CPoint  getCardPos(int index = Player::LastCard) const;
  CRect   getCardHitArea(int) const;
  CRect   getCardPaintArea(int) const;
  CRect   getArea(bool deflatebyOne = false) const;
  CRect   getTextArea() const;
  CString getTooltipText(const CPoint&) const;
  BOOL    resetTooltip(const CPoint& pos) const;
  CChips  getChips() const;
  CChips  getTotalChips() const;
  void    setChips(const CChips&);
  CRect   computeLabelArea(CDC*) const;

  void    setDrawChips(const CChips&);
  CChips  getDrawChips() const;
  BOOL    drawChips(CDC* pDC);
  void    invalidateChips();

public:
  BOOL    isPlayerEligible(int pot, int player);

private:
  enum {
    NumDrawCards = 3,
    CardYOffset  = 20,
    CardXOffset  = 35
  };  

public:

  //
  // Side-pot support
  //
  void pot_ante(Base::Player* player, int slot, const CChips& chips);
  void pot_call(Base::Player* player, int slot, const CChips& chips);
  void pot_raise(Base::Player* player, int slot, const CChips& chips, const CChips& raise);
  void pot_fold(Base::Player* player, int slot);
  void pot_allin(UINT allInSlot, const CChips& to_pay);
  BOOL pot_isComplete() const;
  void pot_anteallin(const CChips& bet);

  SidePot& instantiate_sidepot();
  void add_eligible_player(SidePot&, USHORT player);

  void resetActivePots();
  CRect getPotArea(UINT index);
  void drawSidePots(CDC*, const CRect&) const;
  CString makeTooltip(UINT index);
  void resetSidePots() { sidepots_.clear(); }
  int numSidePots() const;
  CChips getRealRaise() const;

private:
  typedef std::list<SidePot> SidePots;
  SidePots sidepots_;

  void splitPot(CTableView*, UINT allInSlot, Base::Player*, const CChips& to_pay);
  void splitPot(SidePot*, SidePots::iterator&, Base::Player*, int slot, const CChips&, const CChips&);
  void computeSidepotsSize(CDC*, CRect&) const;

  SidePot& mainPot() const;

  int lastTooltip_;
  CSize oldChipsSize_;
  CChips drawChips_;
};

inline SidePot& Community::mainPot() const
{
  return const_cast<SidePot&>(*(sidepots_.begin()));
}

inline int Community::numSidePots() const
{
  int rc = sidepots_.size();

  // Main pot does not count as side pot
  if (rc > 0)
    rc = (rc - 1);

  return rc;
}

inline CChips Community::getDrawChips() const
{
  return drawChips_;
}

// The community player's chips are the Pot.
typedef Community Pot;

#endif