/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class Base::Player
  Created  : 25.10.1999

  OVERVIEW : Class Player is an abstract base class for players. Derive
             concrete players from this class.

 ****************************************************************************/

#include "stdafx.h"
#include "base/player.h"
#include "base/cards.h"
#include "base/graphic_chips.h"
#include "base/draw.h"
#include "ui/tableview.h"
#include "ui/global.h"
#include "ui/registry.h"
#include "common/stringoutput.h"


namespace
{
  const COLORREF g_clrLight = RGB(200, 200, 200);
  const COLORREF g_clrDark  = RGB(250, 250, 250);  
  const int LabelWidth = 80;

  LPCTSTR g_szSittingOut = _T("Sitting Out");
  LPCTSTR g_szFolded     = _T("Folded");

  bool g_bChipsCompartment = true;
};


namespace Base
{


CSize Player::DefaultSize_ = 
  CSize(1.7f * Cards::Width_,
        1.5f * float(Cards::Height_));

BYTE Player::lr_ = 0;
BYTE Player::lg_ = 100;
BYTE Player::lb_ = 0;
BYTE Player::la_ = 100;
BYTE Player::textr_ = 0;
BYTE Player::textg_ = 255;
BYTE Player::textb_ = 0;
BYTE Player::text_backr_ = 10;
BYTE Player::text_backg_ = 10;
BYTE Player::text_backb_ = 10;

BYTE Player::edge_lightr_ = 250;
BYTE Player::edge_lightg_ = 250;
BYTE Player::edge_lightb_ = 250;
BYTE Player::edge_darkr_ = 10;
BYTE Player::edge_darkg_ = 10;
BYTE Player::edge_darkb_ = 10;

BOOL Player::bump_text_ = FALSE;

Player::Player(PlayerSlot& slot, LPCTSTR name, const CChips& chips)
  :
  slot_        (slot),
  drawBackside_(false),
  name_        (name),
  state_       (Base::SitOut),
  chips_       (chips),
  betChips_    (0),
  size_        (DefaultSize_),
  labelSize_   (0, 0),
  chipSize_    (0, 0)
{
  slot_.pPlayer_ = this;
 
  static bool initted = false;
  if (!initted)
  { // first player computes real size
    DefaultSize_ = CSize(int(1.7f * float(Cards::Width_)),
                         Global::GetCharHeight() + Cards::Height_ +
                         4 * Cards::C_Margin + Cards::YOffset_);
    size_ = DefaultSize_;
    initted = true;
  }

  setName(name);
}


Player::~Player()
{
}


//
// Render the player image onto a back
// buffer bitmap and then blt the complete
// image onto 'pDC'.
//
BOOL Player::draw(CDC* pDC, DWORD flags) const
{
  drawImage(pDC, getPos(), flags);
  return TRUE;
}


//
// Render the player image.
//
BOOL Player::drawImage(CDC* pDC, const CPoint& origo, DWORD dwFlags) const
{
  BOOL rc = TRUE;

  CRect area(origo, getSize()); 

  if (drawBounds())
    Base::DrawBumpRect(pDC, area, g_clrLight, g_clrDark);

  // Draw player's cards
  rc = drawCards(pDC, origo, dwFlags);

  // Draw this player's box
  if (rc)
    rc = drawLabels(pDC, origo, dwFlags);
 
  return rc;
}


BOOL Player::drawChips(CDC* pDC)
{
  BOOL rc = TRUE;

  if (betChips_ > 0)
  {
    // If the chip position is anchored to the right,
    // it means the pile will grow to the left.
    // To do this, the position of the leftmost chip
    // is calculated from the size of the chip grapic.

    CPoint pos = slot_.chipPos_;

    if (slot_.chipAnchor_ == PlayerSlot::CA_Right)
    {
      pos.x = slot_.chipPos_.x - chipSize_.cx;
      if (chipSize_.cx == 0)
        pos.x -= Chips::Width_;
    }

    Chips::Instance().drawChips(pDC, betChips_,
                                pos.x, pos.y);
  }

  return rc;
}


void Player::setName(const CString& s)
{
  name_ = s;
  displayName_ = s;

  // If the name is too long for
  // the box, cut it
  CRect r(0, 0, 0, 0);
  CDC dc;
  dc.CreateCompatibleDC(NULL);
  CFont* pOldFont = dc.SelectObject(Global::GetFont());

  Base::DrawBumpText(&dc, (LPCSTR)displayName_, r, DT_CALCRECT, 0, 0, 0);

  while (r.Width() > (LabelWidth + 3) &&
         displayName_.GetLength() > 0)
  {
    // drop one character from the string
    displayName_ = displayName_.Left(displayName_.GetLength() - 1);
    Base::DrawBumpText(&dc, (LPCSTR)displayName_, r, DT_CALCRECT, 0, 0, 0);
  }

  dc.SelectObject(pOldFont);
}


BOOL Player::drawLabels(CDC* pDC, const CPoint& origo, DWORD dwFlags) const
{
  const COLORREF bumpRectLight = RGB(edge_lightr_, edge_lightg_, edge_lightb_);
  const COLORREF bumpRectDark = RGB(edge_darkr_, edge_darkg_, edge_darkb_);
  BOOL bumpText = Player::bump_text_;

  pDC->SetBkMode(TRANSPARENT);
  CFont* pOldFont = pDC->SelectObject(Global::GetBoldFont());

  //
  // Draw player's name
  //

  CRect labelArea(slot_.labelPos_, labelSize_);
  if (labelSize_ == CSize(0, 0))
  { // Recompute label size if it was zeroed
    computeArea(pDC);
    labelArea.right = labelArea.left + labelSize_.cx;
    labelArea.bottom = labelArea.top + labelSize_.cy;
  }

  // Display title with an alpha background
  if (la_ > 0)
  { // fill whole background
    CRect r(labelArea);

    int numCompartments = 1;
    if (g_bChipsCompartment)
        numCompartments += 1;

    r.bottom += (numCompartments * r.Height());
    Base::DrawRectAlpha(pDC, r, lr_, lg_, lb_, la_);
  }
  Base::DrawBumpRect(pDC, labelArea, bumpRectLight, bumpRectDark);

  pDC->SelectObject(Global::GetFont());
  pDC->SetTextColor(RGB(textr_, textg_, textb_));

  if (bumpText)
  {
    Base::DrawBumpText(pDC, (LPCSTR)displayName_,
                       labelArea.TopLeft() + CPoint(2, 0),
                       RGB(textr_, textg_, textb_),
                       RGB(text_backr_, text_backg_, text_backb_));
  }
  else
  {
    CRect r(labelArea);
    r.OffsetRect(2, 0);
    pDC->DrawText(displayName_, r, DT_LEFT);
  }

  //
  // Draw player's last action/state
  //

  pDC->SelectObject(Global::GetBoldFont());
  CRect r(labelArea);
  r.OffsetRect(-1, 1);

  CString s = actionText_;
  COLORREF c = RGB(float(textr_)/1.2f, float(textg_)/1.2f, float(textb_)/1.2f);
  if (actionText_.GetLength() == 0)
  {
    if (state_ & Base::SitOut)
     s = g_szSittingOut;
  }

  labelArea.OffsetRect(0, labelArea.Height());
  Base::DrawBumpRect(pDC, labelArea, bumpRectLight, bumpRectDark);
  labelArea.OffsetRect(0, 1);

  if (bumpText)
  {
    Base::DrawBumpText(pDC, (LPCSTR)s, labelArea, DT_CENTER,
                       c, RGB(text_backr_, text_backg_, text_backb_));
  }
  else
  {
    pDC->DrawText(s, labelArea, DT_CENTER);
  }

  if (g_bChipsCompartment)
  {
    pDC->SelectObject(Global::GetFont());
    labelArea.OffsetRect(0, labelArea.Height() - 1);
    Base::DrawBumpRect(pDC, labelArea, bumpRectLight, bumpRectDark);
    labelArea.OffsetRect(0, 1);

    CStrOut s;
    // XXX OLD s.Format("$%d", getChips());
    s << getChips();
    Base::DrawBumpText(pDC, s.str(), labelArea, DT_CENTER,
                       RGB(255, 255, 0),
                       RGB(text_backr_, text_backg_, text_backb_));
  }

  pDC->SelectObject(pOldFont);

  return TRUE;
}


BOOL Player::drawCards(CDC* pDC, const CPoint& origo, DWORD dwFlags) const
{
  BOOL rc = TRUE;

  if (!(state_ & Base::SitOut) && !(state_ & Base::Fold))
  { // Render the cards
    if (Global::GetGameType() != GT_SevenStud)
    { // in holdem, omaha cards drawn sometimes small,
      // sometimes large
      const_cast<Player*>(this)->drawBackside_ = 
        dwFlags & Cards::C_BackSide;
    }

    int c = 0;
    for (Hand::const_iterator i = cards_.begin(),
                              e = cards_.end();
         rc && i != e;
         ++i, ++c)
    {      
      CPoint pos = makeCardPos(c, Cards::YOffset_);
      rc = Cards::Instance().drawCard(pDC, i->card_,
                                      pos.x, pos.y,
                                      dwFlags);
    }
  }

  return rc;
}



void Player::addCard(const Card& c, AddPos p)
{
  if (p == AddBack)
    cards_.push_back(c);
  else
    cards_.push_front(c);
}


BOOL Player::setCard(int pos, const Card& c)
{
  if (cards_.size() < pos)
    cards_.push_back(c);
  else
  { 
    Hand::iterator p = cards_.begin(),
                   e = cards_.end();
    for (; pos > 0; --pos)
    {
      ++p;
      if (p == e)
        break;
    }

    if (p == e)
      cards_.push_back(c);
    else
    {
      (*p).card_ = c;
    }
  } 
  
  return TRUE;   
}


//
// Remove the last card.
//
BOOL Player::popCard(Card& c)
{
  BOOL rc = TRUE;

  ASSERT(cards_.size() > 0);
  if (cards_.size() > 0)
  { 
    // Get the last card
    Hand::iterator end = cards_.end();
    c = (*(--end)).card_;
    // and pop it out of the deck
    cards_.pop_back();
  }
  else
    rc = FALSE;

  return rc;
}


//
// Remove the 'i'th card
//
BOOL Player::removeCard(int index, Card& c)
{
  BOOL rc = FALSE;

  ASSERT(cards_.size() > index);
  if (cards_.size() > index)
  { // Get to the 'index'th card..
    for (Hand::iterator i = cards_.begin(),
                        e = cards_.end();
         i != e && index > 0;
         ++i, --index)
    {
      ;
    }

    ASSERT(i != e && index == 0);

    // ..and remove it
    if (i != e && index == 0)
    {
      rc = TRUE;
      c = (*i).card_;
      cards_.erase(i);    
    }
  }

  return rc;
}


BOOL Player::getCard(int index, Card& c)
{
  if (cards_.size() > index)
  {
    for (Hand::const_iterator it = cards_.begin(),
                              end = cards_.end();
         it != end && index > 0;
         ++it, --index)
    {
      ; // xxx this would be easier if cards
        // were stored to a vector instead
    }

    ASSERT(it != end && index == 0);

    c = it->card_;
    return TRUE;
  }
  else
    return FALSE;
}


// Empty the hand of this player
void Player::empty()
{
  cards_.clear();
}


void Player::setActionText(LPCTSTR s, BOOL invalidate)
{
  if (actionText_ != s)
  {
    actionText_ = s;
    if (invalidate)
    {
      CTableView* pView = CTableView::Instance();
      if (pView)
        pView->InvalidateRect(getActionTextRect());
    }  
  }
}


CRect Player::getActionTextRect() const
{ // Action text is the compartment below
  // the name compartment
  if (labelSize_.cx == 0 || labelSize_.cy == 0)
    return CRect(0, 0, 0, 0);

  CRect labelArea(slot_.labelPos_, labelSize_);
  labelArea.OffsetRect(0, labelArea.Height()+1);
  labelArea.InflateRect(-2, -2);

  return labelArea;
}


CRect Player::getChipsCompartment() const
{
  // Chips compartment is below the action text 
  if (labelSize_.cx == 0 || labelSize_.cy == 0)
    return CRect(0, 0, 0, 0);

  CRect labelArea(slot_.labelPos_, labelSize_);
  labelArea.OffsetRect(0, 2 * labelArea.Height()+1);
  labelArea.InflateRect(-2, -2);

  return labelArea;
}


int Player::numCards() const
{
  return cards_.size();
}


COLORREF Player::colorLight() const
{
  return g_clrLight;
}


COLORREF Player::colorDark() const
{
  return g_clrDark;
}


CPoint Player::getClosedCardPos(int index) const
{
  // Return backside card pos regardless of
  // how they are currently painted
  Player* ncThis = const_cast<Player*>(this);
  bool old = drawBackside_;
  ncThis->drawBackside_ = true;
  CPoint pos = getCardPos(index);
  ncThis->drawBackside_ = old;
  return pos;
}

//
// Return the upper left corner of
// 'index'th card.
//
CPoint Player::getCardPos(int index) const
{
  int c = index;
  if (c == LastCard)
    if (cards_.size() > 0)
      c = (cards_.size() - 1);
    else
      c = 0;

  if (c > (cards_.size() - 1))
    c = (cards_.size() > 0) ? (cards_.size() - 1) : 0;

  return makeCardPos(c, Cards::YOffset_);
}


CPoint Player::makeCardPos(int index, int yoffset) const
{
  CPoint pos;
  
  if (drawBackside_)
    pos = slot_.backsidePos_;
  else
    pos = slot_.openCardPos_;

  int xoffset = drawBackside_ ? (Cards::XOffset_ / 2) : Cards::XOffset_;

  int xpos = pos.x + index * xoffset + 2;
  //int ypos = pos.y + (1 - index) * yoffset;
  int ypos = pos.y + index * yoffset;

  return CPoint(xpos, ypos);
}


CPoint Player::getChipStartPos() const
{
  return getClosedCardPos(0);
}


CPoint Player::getChipBetPos() const
{
  CPoint pos = slot_.chipPos_;

  if (slot_.chipAnchor_ == PlayerSlot::CA_Right)
  {
    pos.x = slot_.chipPos_.x - chipSize_.cx;
    if (chipSize_.cx == 0)
      pos.x -= Chips::Width_;
  }

  return pos;
}


CRect Player::getChipsArea(int /*index=0*/) const
{
  // The chip position is the top left point,
  // of the first chip. Chips are drawn stacked
  // upwards, hence the area:

  CPoint pos = slot_.chipPos_;
  if (slot_.chipAnchor_ == PlayerSlot::CA_Right)
  {
    pos.x = slot_.chipPos_.x - chipSize_.cx;
    if (chipSize_.cx == 0)
      pos.x -= Chips::Width_;
  }

  int bottomY = pos.y + Chips::Height_;

  CRect r(pos.x,
          bottomY - chipSize_.cy,
          pos.x + chipSize_.cx,
          bottomY);

  r.NormalizeRect();
  r.InflateRect(1, 1);

  return r;
}

CChips Player::getBetChips() const
{
  return betChips_;
}

void Player::setBetChips(const CChips& b)
{
  if (b != betChips_)
  {
    CRect r = getChipsArea();

    betChips_ = b;

    // Recompute chips image size
    chipSize_ = Chips::Instance().getChipsSize(betChips_,
                                               slot_.chipPos_.x,
                                               slot_.chipPos_.y);

    CRect r2 = getChipsArea();
    r2.UnionRect(r2, r);

    CTableView::Instance()->InvalidateRect(&r2);
  }
}


void Player::invalidateChips()
{
}

//
// Return the hit area of 'i'th card.
//
CRect Player::getCardHitArea(int i) const
{
  CPoint pos;
  if (drawBackside_)
    pos = slot_.backsidePos_;
  else
    pos = slot_.openCardPos_;

  if (i == AllCards)
  { // Return area of all cards 
    return CRect(CPoint(pos.x, pos.y + size_.cy - Cards::Height_ - 2),
                 CSize(size_.cx, Cards::Height_));
  }
  else
  { // Return area of specific card
    CPoint ptUpLeft(pos.x + i * Cards::XOffset_ + 2,
                    pos.y + size_.cy - Cards::Height_ - 2);
    CRect cardArea(ptUpLeft, CPoint(ptUpLeft.x + Cards::XOffset_,
                                    ptUpLeft.y + Cards::Height_));

    // Last card area is bigger
    if (i == (cards_.size() - 1))
    {
      cardArea = CRect(ptUpLeft, CPoint(ptUpLeft.x + Cards::Width_,
                                        ptUpLeft.y + Cards::Height_));
    }

    return cardArea;
  }
}


//
// Return the paint area of 'i'th card.
//
CRect Player::getCardPaintArea(int i) const
{
  CPoint pos;
  CSize  size;
  if (drawBackside_)
  {
    pos = slot_.backsidePos_;
    size = CSize(Cards::BacksideCardWidth_,
                 Cards::BacksideCardHeight_);
  }
  else
  {
    pos = slot_.openCardPos_;
    size = CSize(Cards::OpenCardWidth_,
                 Cards::OpenCardHeight_);
  }

  if (i == AllCards)
  { // Return area of all cards
    if (cards_.size() > 0)
    {
      size.cx += ((cards_.size() - 1) * (Cards::XOffset_ + 2));
      size.cy += (cards_.size() - 1) * Cards::YOffset_;
    }
    return CRect(pos, size);
  }
  else if (i == LastCard)
  { // Return area of last card
    i = numCards() - 1;
  }

  int width = drawBackside_ ? Cards::BacksideCardWidth_ : Cards::OpenCardWidth_;
  int height = drawBackside_ ? Cards::BacksideCardHeight_ : Cards::OpenCardHeight_;

  return CRect(makeCardPos(i, Cards::YOffset_), CSize(width, height));
}


CRect Player::getArea(bool deflateByOne) const
{
  CRect r1(getTextArea());
  CRect r2(getCardPaintArea(Player::AllCards));
  CRect r3(getChipsArea());

  CRect rU;
  rU.UnionRect(r1, r2);
  rU.UnionRect(rU, r3);

  return rU;
}


CRect Player::getTextArea() const
{
  if (labelSize_ == CSize(0, 0))
  { // Recompute label size
    CDC dc;
    dc.CreateCompatibleDC(NULL);
    CFont* pOldFont = dc.SelectObject(Global::GetBoldFont());
    computeArea(&dc);
    dc.SelectObject(pOldFont);
  }

  // The text area is two or three compartments
  CSize s(labelSize_);

  if (g_bChipsCompartment)
    s.cy *= 3;
  else
    s.cy *= 2;

  return CRect(slot_.labelPos_, s);
}


CString Player::getTooltipText(const CPoint& p) const
{
  CString s;

  CRect r = getChipsArea();
  
  if (!r.PtInRect(p))
  { // Show player's full name in tooltip 
    // if it doesn't fit the box
    if (displayName_.GetLength() != name_.GetLength())
    {
      s.Format("%s\n", (LPCSTR)name_);
    }

    CStrOut tmp;
    CString city = getHomeCity();
    if (!city.IsEmpty())
    {
      tmp << "Chips: " << getChips() << "\nFrom: " << getHomeCity();
      // XXX OLD tmp.Format("Chips: $%9.2f\nFrom: %s",
      //           getChips().asDouble(), getHomeCity());
      s += tmp.str();
    }
    else
    {
      tmp << "Chips: " << getChips();
      // XXX OLD tmp.Format("Chips: $%9.2f", getChips().asDouble());
      s += tmp.str();
    }
  }
  else
  { // The mouse is on top of the chips
    s.Format("$%9.2f", betChips_);
  }

  return s;
}


// By default a Player has one tooltip; subclass
// can override to implement multiple tooltips per
// Player.
BOOL Player::resetTooltip(const CPoint&) const
{
  return FALSE;
}


void Player::computeArea(CDC* pDC) const
{
  CRect r(0, 0, 0, 0);
  CString title;
  title.Format("%s $%d", name_, chips_);
  Base::DrawBumpText(pDC, title, r, DT_CALCRECT, 0, 0, 0);
  r.InflateRect(1, 1);
  r.right = r.left + LabelWidth;
  const_cast<Player&>(*this).labelSize_ = r.Size();
}


//
// Return the index of the first selected card.
//
int Player::firstSelectedCard() const
{
  int sel = 0;

  for (Hand::const_iterator i = cards_.begin(),
                            e = cards_.end();
       i != e;
       ++i, ++sel)
  {
    if ((*i).selected_)
      return sel;
  }

  return NoSelection;
}


//
// Return the index of the last selected card.
//
int Player::lastSelectedCard() const
{
  ASSERT(FALSE);
  return NoSelection;
}


//
// Decide which card got hit and toggle its
// selection status.
//
void Player::mouseClick(const CPoint& ptHit)
{
  int c = 0;

  for (Hand::iterator i = cards_.begin(),
                      e = cards_.end();
       i != e;
       ++i, ++c)
  {
    if (hitTest(c, ptHit))
    { // hit - toggle card's selection status
      
      i->selected_ = !i->selected_;

      CTableView::Instance()->InvalidateRect(getCardPaintArea(c));
      break;
    }
  }
}


BOOL Player::hitTest(int cardNum, const CPoint& ptHit) const
{
  CRect r = getCardHitArea(cardNum);
  BOOL rc = r.PtInRect(ptHit);

#if 0
  ASSERT(cardNum >= 0 && cardNum < NumCards);

  if (!rc && select_[cardNum])
  { // If it is a selected card, must check
    // upper horizontal slice as well
    r.bottom = r.top + CardSelectOffset;
    r.right = r.left + Cards::Width_;

    // If next card or the one after that is
    // also selected, must exclude its area
    if (cardNum < (NumCards - 1) && select_[cardNum + 1])
      r.right = r.left + Cards::XOffset_;
    else if (cardNum < (NumCards - 2) && select_[cardNum + 2])
      r.right = r.left + 2 * Cards::XOffset_;

    rc = r.PtInRect(ptHit);
  }
#endif

  return rc;
}


void Player::setFolded(BOOL b)
{
  if (b)
    state_ |= Base::Fold;
  else
    state_ &= ~Base::Fold;
}


void Player::setSitIn(BOOL b)
{
  if (b)
    state_ |= Base::SitIn;
  else
    state_ &= ~Base::SitIn;  
}


BOOL Player::drawBounds() const
{
  static BOOL doIt = Registry::GetDrawPlayerBounds(FALSE);
  return doIt;
}


void Player::copyDeck(std::vector<Card>& d)
{
  for (Hand::const_iterator i = cards_.begin(),
                            e = cards_.end();
       i != e;
       ++i)
  {
    d.push_back(i->card_);
  }
}


void Player::setDeck(const std::vector<Card>& d)
{
  cards_.clear();

  for (std::vector<Card>::const_iterator i = d.begin(),
                                         e = d.end();
       i != e;
       ++i)
  {
    cards_.push_back(*i);
  }
}


void Player::SetLabelAlpha(BYTE r, BYTE g, BYTE b, BYTE a)
{
  lr_ = r;
  lg_ = g;
  lb_ = b;
  la_ = a;

  // compute label edge colors based on background
  edge_lightr_ = min(1.7f * float(lr_), 255);
  edge_lightg_ = min(1.7f * float(lg_), 255);
  edge_lightb_ = min(1.7f * float(lb_), 255);

  edge_darkr_ = 0.2f * float(lr_);
  edge_darkg_ = 0.2f * float(lg_);
  edge_darkb_ = 0.2f * float(lb_);
}


void Player::SetTextColor(BYTE r, BYTE g, BYTE b)
{
  textr_ = r;
  textg_ = g;
  textb_ = b;

  if (r == 0 && g == 0 && b == 0)
  { // black text => use gray shadow
    text_backr_ = 100;
    text_backg_ = 100;
    text_backb_ = 100;
  }
}


void Player::SetTextDark(BYTE r, BYTE g, BYTE b)
{
  text_backr_ = r;
  text_backg_ = g;
  text_backb_ = b;
}


void Player::SetBumpText(BOOL b)
{
  bump_text_ = b;
}


void Player::SetEdgeLight(BYTE r, BYTE g, BYTE b)
{
  edge_lightr_ = r;
  edge_lightg_ = g;
  edge_lightb_ = b;
}


void Player::SetEdgeDark(BYTE r, BYTE g, BYTE b)
{
  edge_darkr_ = r;
  edge_darkg_ = g;
  edge_darkb_ = b;
}
  

CChips Player::getChips() const
{ 
  return chips_;
}


void Player::setChips(const CChips& c)
{
  chips_ = c;
  if (g_bChipsCompartment)
  {
      CTableView* table = CTableView::Instance();
      if (table)
      {
        table->InvalidateRect(&getChipsCompartment());
      }
  }
}


} // Base