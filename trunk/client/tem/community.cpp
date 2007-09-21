/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class Community
  Created  : 25.10.1999

  OVERVIEW : Implements the Community player for Texas Hold'em.

 ****************************************************************************/

#include "stdafx.h"
#include "tem/community.h"
#include "base/cards.h"
#include "base/draw.h"
#include "base/loopguard.h"
#include "base/graphic_chips.h"
#include "ui/global.h"
#include "ui/tableview.h"
#include "ui/chatview.h"
#include "network/network.h"
#include "common/stringoutput.h"

namespace
{
  
LPCTSTR g_szName = _T("Pot");
const int Margin = 5;

void ClearEligible(std::vector<int>& v)
{
  for (int i = 0; i < CTableView::MaxPlayers; i++)
    v[i] = FALSE;
}


//
// Return the number of players currently
// eligible in 'old_eligible', and make
// those players eligible to 'sidepot'.
//
UINT AddEligiblePlayers(CTableView*    pTable,
                        const SidePot& old_pot,
                        SidePot&       new_sidepot,
                        UINT           allin_slot)
{
  UINT players = 0;
  int allin = allin_slot;

  for (int i = 0; i < CTableView::MaxPlayers; i++)
  {
    if (old_pot.eligible_players_[i] && i != allin)
    {
      new_sidepot.addEligible(i);
      players++;
    }
  }

  return players;
}


//
// Build a tip text string that contains the
// names of the players eligible for this pot.
//
void MakeTipText(const SidePot& pot,
                 int            pot_num,
                 CString&       tipText,
                 CTableView*    pTable)
{
  if (pot_num == 0)
    tipText.Format("Main Pot\nEligible players:");
  else
    tipText.Format("Side-Pot %d\nEligible players:", pot_num);

  for (int i = 0; i < CTableView::MaxPlayers; i++)
  {
    if (pot.eligible_players_[i])
    {
      Player* pPlayer = pTable->getPlayer(i);
      if (pPlayer)
      {
        tipText += CString("\n") + pPlayer->getName();
      }
    }
  }
}


}

using Base::Cards;
using Base::Card;
using Base::Deck;


Community::Community(PlayerSlot& slot)
  :
  Base::Player  (slot, g_szName, 0),
  lastTooltip_  (-1),
  oldChipsSize_ (0, 0),
  drawChips_    (0)
{
  // Community size is different from other players
  size_ = CSize(2 * Margin + 5 * Base::Cards::Width_, // room for 5 cards
                LONG(1.3f * float(Cards::Height_)));

  drawBackside_ = false;

  SidePot mainPot;
  sidepots_.push_back(mainPot);
}


BOOL Community::drawImage(CDC* pDC, const CPoint& origo, DWORD dwFlags) const
{
  BOOL rc = TRUE;

  // Display title
  CStrOut s;
  s << name_ << ' ' << mainPot().get_chips();

  CString title(s.str());

  pDC->SetTextColor(RGB(textr_, textg_, textb_));
  pDC->SetBkMode(TRANSPARENT);
  CFont* pOldFont = pDC->SelectObject(Global::GetBoldFont());

  CRect labelArea = computeLabelArea(pDC);

  if (la_ > 0)
  { // fill whole background
    Base::DrawRectAlpha(pDC, labelArea, lr_, lg_, lb_, la_);
  }

  if (Player::bump_text_)
  {
    Base::DrawBumpText(pDC, (LPCSTR)title,
                       labelArea.TopLeft() + CPoint(1, 0),
                       RGB(textr_, textg_, textb_),
                       RGB(text_backr_, text_backg_, text_backb_));
  }
  else
  {
    pDC->DrawText(title, labelArea, DT_LEFT);
  }

  // Render the cards
  int c = 0;
  for (Hand::const_iterator i = cards_.begin(),
                            e = cards_.end();
         rc && i != e;
         ++i, c++)
  {
    CPoint pos = getCardPos(c);
    rc = Cards::Instance().drawCard(pDC, i->card_,
                                    pos.x, pos.y,
                                    dwFlags);
  }

  drawSidePots(pDC, labelArea);

  pDC->SelectObject(pOldFont);

  return rc;
}


void Community::setDrawChips(const CChips& c)
{
  if (drawChips_ != c)
  {
    drawChips_ = c;
    oldChipsSize_ = chipSize_;
    chipSize_ = Base::Chips::Instance().getChipsSize(
                                        drawChips_,
                                        slot_.chipPos_.x,
                                        slot_.chipPos_.y);

    invalidateChips();
  }
}

BOOL Community::drawChips(CDC* pDC)
{
  CChips chips = drawChips_;

  if (chips > 0)
  {
      Base::Chips::Instance().drawChips(
                                pDC, chips,
                                slot_.chipPos_.x,
                                slot_.chipPos_.y);

  }

  return TRUE;
}

//
// Return position of 'index'th card
// in world coordinates.
//
CPoint Community::getCardPos(int index) const
{ 
  int c = index;
  if (c == LastCard)
  {
    if (cards_.size() > 0)
      c = (cards_.size() - 1);
    else
      c = 0;
  }

  if (c > (cards_.size() - 1))
    c = (cards_.size() > 0) ? (cards_.size() - 1) : 0;

  CPoint p(slot_.openCardPos_.x + c * Cards::OpenCardWidth_ + Margin,  
           slot_.openCardPos_.y);

  return p;
}


CRect Community::getArea(bool deflatebyOne) const
{
  CRect label = computeLabelArea(NULL);
  CRect cards(slot_.openCardPos_,
              CSize(cards_.size() * Cards::OpenCardWidth_,
                    Cards::OpenCardHeight_));
  CRect rU;
  rU.UnionRect(label, cards);
  return rU;
}



//
// Return the hit area of 'i'th card.
//
CRect Community::getCardHitArea(int i) const
{
  if (i == AllCards)
  { // Return area of all cards 
    return CRect(CPoint(slot_.pos_.x, slot_.pos_.y + size_.cy - Cards::Height_ - 2),
                 CSize(size_.cx, Cards::Height_));
  }
  else
  {
    if (i == LastCard)
      i = cards_.size() > 0 ? (cards_.size()  - 1) : 0;

    ASSERT(i >= 0 && i < cards_.size());

    // Return area of specific card
    CPoint ptUpLeft(slot_.pos_.x + i * Cards::Width_ + Margin,
                    slot_.pos_.y + size_.cy - Cards::Height_ - 2);

    return CRect(ptUpLeft, CPoint(ptUpLeft.x + Cards::Width_,
                                  ptUpLeft.y + Cards::Height_));
  }
}


//
// Return the paint area of 'i'th card.
//
CRect Community::getCardPaintArea(int i) const
{
  if (i == AllCards)
  { // Return area of all cards 
    return CRect(getCardPos(0),
                 CSize(numCards() * (Cards::Width_ + Margin), Cards::Height_));
  }
  else
  {
    if (i == LastCard)
      i = cards_.size() > 0 ? (cards_.size()  - 1) : 0;

    CPoint pos = getCardPos(i);
    return CRect(pos, CSize(Cards::Width_, Cards::Height_));
  }
}

// Override since community has
// different label area.
CRect Community::getTextArea() const
{
  return computeLabelArea(NULL);
}


CRect Community::computeLabelArea(CDC* pDC) const
{
  CRect area(0, 0, 0, 0);

  if (labelSize_ == CSize(0, 0))
  { // Recompute label size
    CDC dc;
    CFont* pOldFont = NULL;
    if (!pDC)
    {
      dc.CreateCompatibleDC(NULL);
      pOldFont = dc.SelectObject(Global::GetBoldFont());
      pDC = &dc;
    }

    CString title;
    title.Format("%s $%i", name_, mainPot().get_chips());

    Base::DrawBumpText(pDC, title, area, DT_CALCRECT, 0, 0, 0);
    const_cast<Community&>(*this).labelSize_ = area.Size();

    CRect r(area.TopLeft(), CSize(0, 0));
    computeSidepotsSize(pDC, r);
    int max_width = max(labelSize_.cx, r.Width());
    const_cast<Community&>(*this).labelSize_.cx = max_width;

    if (dc.GetSafeHdc())
      dc.SelectObject(pOldFont);
  }

  area = CRect(slot_.labelPos_, labelSize_);

  // If there are more than 1 side-pots, the
  // main pot needs to be drawn (n - 1) times
  // higher to avoid the side-pots from going
  // over the community cards.
  if (numSidePots() > 1)
  {
    int n = numSidePots() - 1;
    area.OffsetRect(0, -n * area.Height());
    // must also prevent it from going above
    // client area top
    if (area.top < 0)
      area.OffsetRect(0, -area.top);
  }

  // If there are side pots, the label area
  // is bigger.
  area.bottom += numSidePots() * area.Height();

  return area;
}


CString Community::getTooltipText(const CPoint& pt) const
{
  CTableView* pTable = CTableView::Instance();
  ASSERT(pTable);

  CString tipText;

  CRect r = getChipsArea();
  if (r.PtInRect(pt))
  {
    tipText.Format("$%d", drawChips_);
    return tipText;
  }

  r = computeLabelArea(NULL);
  r.bottom = r.top + labelSize_.cy;

  // Find the pot containing 'pt' and
  // make a tip text for it.
  int pot_num = 0;
  for (SidePots::const_iterator it = sidepots_.begin(),
                                end = sidepots_.end();
       it != end;
       ++it, ++pot_num)
  {
    if (r.PtInRect(pt))
    {
      MakeTipText(*it, pot_num, tipText, pTable);
      break;
    }
    r.OffsetRect(0, r.Height() - 1);
  }

  return tipText;
}


BOOL Community::resetTooltip(const CPoint& pos) const
{
  Community* ncThis = const_cast<Community*>(this);
  BOOL rc = FALSE;

  UINT index = 0;
  int pots = numSidePots() + 1; // 1 is main pot
  for (int i = 0; i < pots; i++)
  {
    CRect r = ncThis->getPotArea(i);
    if (r.PtInRect(pos))
      break;
  }

  // reset if 'pos' on different pot than
  // previous
  if (i < pots && i != lastTooltip_)
  {
    rc = TRUE;
    ncThis->lastTooltip_ = i;
  }
  
  return rc;
}


CChips Community::getChips() const
{
  return mainPot().get_chips();
}


CChips Community::getTotalChips() const
{
  CChips chips;

  for (SidePots::const_iterator it = sidepots_.begin(),
                                end = sidepots_.end();
       it != end;
       ++it)
  {
    chips += (*it).get_chips();
  }

  return chips;
}

void Community::setChips(const CChips& chips)
{ 
  CRect r = getChipsArea();
  CTableView::Instance()->InvalidateRect(&r);  

  if (chipSize_.cx != 0 && chipSize_.cy != 0)
    oldChipsSize_ = chipSize_;
  drawChips_ = 0;

  // setChips(0) is the only meaningful operation;
  // others must go through the pot_xxx functions
  ASSERT(chips == 0);
  if (chips == 0)
  { // Pot is reset - remove all side-pots
    sidepots_.clear();
    
    // reset main pot
    SidePot pot;
    sidepots_.push_back(pot);
    mainPot().reset();
    mainPot().isActive_ = TRUE;
  }  

  // Reset label size so it'll be repainted
  labelSize_ = CSize(0, 0);
}

bool gbInAnte = false;

//
// Pot & side pot support
//
void Community::pot_ante(Player* player, int slot, const CChips& pay)
{
    gbInAnte = true;

    // In tournaments we have to handle the case
    // of player not having enough chips for the ante

    CChips bigBlind = CTableView::Instance()->lolimit();

    bool isSmallBlind = (pay != bigBlind);
    if (isSmallBlind)
    {
        mainPot().bet_ = pay;
        pot_call(player, slot, pay);
    }
    else
    {
        // Paying big blind is similar to raise -
        // clear eligible lists first
        for (SidePots::iterator it = sidepots_.begin(),
                                end = sidepots_.end();
             it != end;
             ++it)
        {
            (*it).clearEligible();
        }

        if (sidepots_.size() > 1)
        {
            // Pot has already been split - last pot
            // gets new bet

            SidePots::iterator it = sidepots_.begin();
            SidePot& mainPot = (*it++);
            SidePot& pot2 = (*it);
            pot2.bet_ = bigBlind - mainPot.bet_;
        }
        else
        {
            mainPot().bet_ = bigBlind;
        }
        pot_call(player, slot, pay);
    }

    gbInAnte = false;
}


void Community::pot_call(Player* player, int slot, const CChips& cc_call_amount)
{
  CChips call_amount(cc_call_amount);

#ifdef DEBUG_POT_
  CChatView* pChat = CChatView::Instance();
  CString s;
  s.Format("--- pot_call: %s %d", player->getName(), call_amount);
  pChat->addDebugMessage(s, 0);
#endif

  bool debug1 = false;

  if (sidepots_.size() == 1)
  { // we have only the main pot
    mainPot().add_chips(slot, call_amount);
    mainPot().addEligible(slot);
#ifdef DEBUG_POT_
    mainPot().dump(0);
#endif
  }
  else
  { 
    // go through pots and distribute payment
    int pot_num = 1;
    for (SidePots::iterator it = sidepots_.begin(),
                            end = sidepots_.end();
         it != end;
         ++it, ++pot_num)
    {
      if (it->isActive_)
      {
        // If we've gone a round the pot must be deactivated
        // unless its the last live pot
        if (it->isEligible(slot) && (pot_num < sidepots_.size()))
        {
#ifdef DEBUG_POT_
          pChat->addDebugMessage("--- pot_call: Deactivating pot", pot_num - 1);
#endif
          it->isActive_ = FALSE;
          debug1 = true;
        }
        else if (call_amount != 0)
        {
          CChips pay_this_pot = it->bet_ - it->chips_[slot];

          // Detect wrap-around!!!
          //SYSMSG_ASSERT(pay_this_pot < SHRT_MAX);
          //ASSERT(pay_this_pot < SHRT_MAX);

          //SYSMSG_ASSERT(pay_this_pot >= 0);
          //ASSERT(pay_this_pot >= 0);

          it->add_chips(slot, pay_this_pot);

          //SYSMSG_ASSERT(call_amount >= pay_this_pot);
          //ASSERT(call_amount >= pay_this_pot);

          call_amount -= pay_this_pot;   
        }

        it->addEligible(slot);

#ifdef DEBUG_POT_
        it->dump(pot_num);
#endif
      }
    }

    // When we've gone through all pots,
    // 0 chips should remain
    //SYSMSG_ASSERT(call_amount == 0);
    //ASSERT(call_amount == 0); // XXX tournament! this assert fails
  }

  // Reset label size so it'll be repainted
  labelSize_ = CSize(0, 0);
}


void Community::pot_raise(Player* player, int slot, const CChips& current_bet, const CChips& raise)
{
  if (sidepots_.size() > 1)
  { 
    if (mainPot().isEligible(slot))
    {
#ifdef DEBUG_POT_
      CChatView::Instance()->addDebugMessage("--- pot_raise: Deactivating main pot", 0);
#endif
      mainPot().isActive_ = FALSE;
    }

    // raising means live side pot's bet is raised
    // (its last in the list)
    SidePot& livePot = sidepots_.back();
    livePot.bet_ += raise;

    // Eligible list in live pot is cleared
    ClearEligible(livePot.eligible_players_);
  }
  else
  { // we have only main pot
    mainPot().bet_ += raise;
    ClearEligible(mainPot().eligible_players_);
  }

  // Set completed flag if necessary
  CChips realRaise = getRealRaise();
  SidePot& livePot = sidepots_.back();
  if (livePot.isComplete_)
  {
    if (realRaise > raise)
    {
      livePot.isComplete_ = FALSE;
      livePot.betComplete_ = realRaise - raise;
    }
  }
  else
  { 
    if (raise == livePot.betComplete_)
    {
      livePot.isComplete_ = TRUE;
      livePot.betComplete_ = 0;
    }
  }
 

  // Calling will subtract the correct amount
  // and will make the player eligible
  pot_call(player, slot, current_bet + raise);
}


void Community::pot_fold(Base::Player*, int slot)
{
  // remove player from all eligible lists
  for (SidePots::iterator it = sidepots_.begin(),
                          end = sidepots_.end();
       it != end;
       ++it)
  {
    it->eligible_players_[slot] = FALSE;
  }
}

BOOL Community::pot_isComplete() const
{
  if (sidepots_.size() > 0)
    return sidepots_.back().isComplete_;
  else
    return TRUE;
}

SidePot& Community::instantiate_sidepot()
{
  SidePot sidepot;
  sidepots_.push_back(sidepot);
  return *(sidepots_.rbegin()); // return the new sidepot
}


void Community::add_eligible_player(SidePot& sidepot, USHORT player)
{
  sidepot.eligible_players_[player] = TRUE;
}


void Community::pot_allin(UINT allin_slot, const CChips& to_pay)
{
  CTableView* pTable = CTableView::Instance();
  ASSERT(pTable);
  Player* pPlayer = NULL;
  if (pTable)
    pPlayer = pTable->getPlayer(allin_slot);
  if (!pPlayer)
    return; // Might happen if player connection
            // dies at suitable moment?

  splitPot(pTable, allin_slot, pPlayer, to_pay);
}

void Community::pot_anteallin(const CChips& bet)
{
    // A player is about to go all-in in the ante.
    // Set the active pot's bet accordingly.
    if (sidepots_.size() > 1)
    {
        // Last pot's bet is set
        (*sidepots_.rbegin()).bet_ = bet;
    }
    else
    {
        mainPot().bet_ = bet;
    }
}


//
// The current live pot is split making a new
// side pot that becomes the live pot.
// Excess chips are moved from the current live
// pot to the new side pot.
//
void Community::splitPot(CTableView*   pTable,
                         UINT          slot,
                         Player*       player,
                         const CChips& pay)
{
    bool wasSplit = false;

    // Take the chips from the player now - 
    // if this is network allin, 'pay' will be
    // smaller - otherwise player's chips should
    // be smaller
    CChips chips = minimum(player->getChips(), pay);
    player->setChips(player->getChips() - chips);

    SidePot* pot = &mainPot();
    SidePots::iterator it = sidepots_.begin(), end;

    // find the pot which will be split (if any),
    // and distribute chips as we go
    if (sidepots_.size() > 1)
    {

    // XXX
    // Leave the last pot unhandled so it will be split
    // if no other pot will be split
    int numPots = sidepots_.size();

    for (it = sidepots_.begin(), end = sidepots_.end();
         it != end; ++it)
    {
        pot = &(*it);

        // Quit loop in last pot
        if (numPots-- == 1)
            break;

        if (pot->isActive_)
        {
            // player has to pay the difference between what
            // he has paid so far and the current pot bet
            ASSERT(pot->bet_ >= pot->chips_[slot]);
            CChips pay_this_pot = pot->bet_ - pot->chips_[slot];

            if (chips >= pay_this_pot)
            {   //
                // player can match this pot's bet - pay it,
                // make player eligible, and continue to next pot
                //
                pot->chips_[slot] += pay_this_pot;
                chips -= pay_this_pot;
                pot->addEligible(slot);
            }
            else
            {
                //
                // player cannot match this pot's bet -
                // if player has chips left, split the pot,
                // and we're done
                // 
                if (chips > 0)
                {
                    // okay split this pot
                    splitPot(pot, it, player, slot, chips, pay_this_pot);
                    pot->addEligible(slot);
                    chips = 0;
                    wasSplit = true;
                }
                else
                {
                    // player's chips matched evenly, no need to split
                    // any pot
                    wasSplit = true;
                }

                // we're done, exit loop
                break;
            }

        }
    }
    }

    if (!wasSplit)
    {
        // Split the last pot 
        ASSERT(pot->isActive_); // last pot must be active
        ASSERT(pot->bet_ >= pot->chips_[slot]);
        CChips pay_this_pot = pot->bet_ - pot->chips_[slot];
        splitPot(pot, it, player, slot, chips, pay_this_pot);
        pot->addEligible(slot);
    }

  // Force recomputation of label area
  labelSize_ = CSize(0, 0);
}

void Community::splitPot(SidePot* oldPot,
                         SidePots::iterator& pos,
                         Base::Player* player,
                         int slot,
                         const CChips& player_chips,
                         const CChips& pay_this_pot)
{
    // Create new pot and insert it after 'oldPot'
    SidePot newPot;

    // Copy bet completion state to new pot -
    // in case of raise & allin, this handles completion
    // state to new pot
    newPot.isComplete_ = oldPot->isComplete_;
    newPot.betComplete_ = oldPot->betComplete_;

    // Make players eligible to oldPot eligible to newly
    // splitted pot
    int i = 0;
    for (i= 0; i < CTableView::MaxPlayers; i++)
    {
        if (oldPot->isEligible(i))
            newPot.addEligible(i);
    }

    // Move excess chips from old pot to newly splitted pot

    // How many chips is the player short of?
    //ASSERT(pay_this_pot >= player_chips);

    CChips missing_chips;
    
    if (pay_this_pot >= player_chips)
      missing_chips = pay_this_pot - player_chips;

    // oldPot gets all of players chips
    oldPot->chips_[slot] += player_chips;

    // side pot bet becomes what the allin player
    // was not able to pay (ie the excess chips)
    newPot.bet_ = missing_chips;

    // old live pot's bet becomes what the
    // all-in player did pay
    CChips allin_chips_in_pot = oldPot->chips_[slot];
    oldPot->bet_ = allin_chips_in_pot;

    for (i = 0; i < CTableView::MaxPlayers; i++)
    {
        if (oldPot->chips_[i] > allin_chips_in_pot)
        {   // this player has more in pot than the allin player was able to
            // afford - move excess chips to side pto
            CChips excess_chips = (oldPot->chips_[i] - allin_chips_in_pot);
            newPot.add_chips(i, excess_chips);
            oldPot->chips_[i] -= excess_chips;
        }
    }

    oldPot->isLivePot_ = FALSE;

    // if the new pot is the last pot, it becomes
    // the live pot

    // insert after pos
    if (pos == sidepots_.end())
    {   // Insert pot last
        newPot.isLivePot_ = TRUE;
        sidepots_.push_back(newPot);
    }
    else
    {
        if (++pos == sidepots_.end())
        {
            newPot.isLivePot_ = TRUE;
            sidepots_.push_back(newPot);
        }
        else
            sidepots_.insert(pos, newPot);
    }

#ifdef DEBUG_POT_
    oldPot->dump(0);
    newPot.dump(1);
#endif
}


// 0 : main pot
// 1 - n : i'th side-pot
CRect Community::getPotArea(UINT index)
{
  CRect area = computeLabelArea(NULL);
  area.bottom = area.top + labelSize_.cy;
  area.OffsetRect(0, index * area.Height());
  return area;
}


void Community::drawSidePots(CDC* pDC, const CRect& labelArea) const
{
  if (sidepots_.size() <= 1)
    return; // no side pots

  CRect r(labelArea); // contains all pots
  r.bottom = r.top + labelSize_.cy;
  int potnum = 1;
  CStrOut s;

  SidePots::const_iterator it = sidepots_.begin(),
                           end = sidepots_.end();

  // begin from second pot which is first side pot
  for (++it; it != end; ++it, ++potnum)
  {
    s << CStrOut::Clear
      << "Side-Pot " << potnum << ' ' << it->get_chips();
    r.OffsetRect(0, r.Height() - 1);
    if (Player::bump_text_)
    {
      Base::DrawBumpText(pDC, s.str(), r.TopLeft(),
                         RGB(textr_, textg_, textb_),
                         RGB(text_backr_, text_backg_, text_backb_));

    }
    else
    {
      pDC->DrawText(s.str(), r, DT_LEFT);
    }
  }
}


void Community::computeSidepotsSize(CDC* pDC, CRect& r) const
{
  if (sidepots_.size() <= 1)
    return; // no side pots

  CString s;
  USHORT potnum = 0;

  SidePots::const_iterator it = sidepots_.begin(),
                           end = sidepots_.end();

  // begin from second pot which is first side pot
  for (++it; it != end; ++it, ++potnum)
  {
    CRect area(r.TopLeft(), CSize(0, 0));

    s.Format("Side-Pot %d $%d", potnum, it->get_chips());
    Base::DrawBumpText(pDC, s, area,
                       DT_CALCRECT, 0, 0);
    int width = max(r.Width(), area.Width());
    r.right = r.left + width;
    r.bottom += area.Height();
  }
}


BOOL Community::isPlayerEligible(int pot, int player)
{
  CTableView* pTable = CTableView::Instance();
  ASSERT(pTable);

  BOOL rc = FALSE;

  if (pot == 0)
  { // main pot
    Player* pPlayer = pTable->getPlayer(player);
    if (pPlayer && pPlayer->isInGame())
      rc = TRUE;
  }
  else
  { // XXX side pots
  }

  return rc;
}

//
// SidePot mfunctions
//
SidePot::SidePot()
  :
  bet_      (0),
  isLivePot_(FALSE),
  isActive_ (TRUE),
  isComplete_(TRUE),
  betComplete_(0),
  area_     (0, 0, 0, 0),
  eligible_players_(CTableView::MaxPlayers)
{
  clear_chips();
  ClearEligible(eligible_players_);
}

  
SidePot::SidePot(const CChips& bet)
  :
  bet_      (bet),
  isLivePot_(FALSE),
  isActive_ (TRUE),
  isComplete_(TRUE),
  betComplete_(0),
  area_     (0, 0, 0, 0),
  eligible_players_(CTableView::MaxPlayers)
{
  clear_chips();
  ClearEligible(eligible_players_);
}

  
SidePot::SidePot(const SidePot& sp)
  :
  bet_      (sp.bet_),
  isLivePot_(sp.isLivePot_),
  isActive_ (sp.isActive_),
  isComplete_(sp.isComplete_),
  betComplete_(sp.betComplete_),
  area_     (sp.area_),
  eligible_players_(sp.eligible_players_)
{
  memcpy(chips_, sp.chips_, sizeof(chips_));
}


SidePot& SidePot::operator = (const SidePot& rhs)
{
  if (this != &rhs)
  {
    bet_ = rhs.bet_;
    memcpy(chips_, rhs.chips_, sizeof(chips_));
    isLivePot_= rhs.isLivePot_;
    isActive_ = rhs.isActive_;
    isComplete_ = rhs.isComplete_;
    betComplete_ = rhs.betComplete_;
    area_ = rhs.area_;
    eligible_players_ = rhs.eligible_players_;
  }
  return *this;
}


void SidePot::addEligible(int slot)
{
  eligible_players_[slot] = TRUE;
}


void SidePot::clearEligible()
{
  ClearEligible(eligible_players_);
}


#ifdef DEBUG_POT_
void SidePot::dump(int pot_num)
{
  CChatView* pChat = CChatView::Instance();
  CString s;
  s.Format("Pot[%i] sum: %d bet: %d active: %d isCmplt: %d, betCmplt: %d",
           pot_num, get_chips(), bet_,
           isActive_, isComplete_, betComplete_);
  pChat->addDebugMessage(s, 0);
  s = "Chips:";

  for (int i = 0; i < CTableView::MaxPlayers; i++)
  {
    CString tmp;
    tmp.Format(" %d", chips_[i]);
    s += tmp;
  }
  pChat->addDebugMessage(s, 0);
}
#endif


CChips Community::getRealRaise() const
{
  CTableView* pTable = CTableView::Instance();
  if (!pTable) return 0;

  if (numCards() < 4)
    return pTable->lolimit();
  else
    return pTable->hilimit();      
}


void Community::invalidateChips()
{
  // Compute the last chip area 
  CSize tmp = chipSize_;
  chipSize_ = oldChipsSize_;
  CRect r1 = getChipsArea();

  // Compute new chip area
  chipSize_ = tmp;
  chipSize_ = Base::Chips::Instance().getChipsSize(
                                          drawChips_,
                                          slot_.chipPos_.x,
                                          slot_.chipPos_.y);

  CRect r2 = getChipsArea();
  r2.UnionRect(&r1, &r2);

  CTableView::Instance()->InvalidateRect(&r2);
}