#ifndef __base_player_h__
#define __base_player_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class Base::Player
  Created  : 25.10.1999

  OVERVIEW : Class Player is an abstract base class for players. Derive
             concrete players from this class.

 ****************************************************************************/



#include <string>
#include <vector>
#include "base/def.h"
#include "base/playerslot.h"
#include "base/playerstate.h"
#include "common/chips.h"

namespace Base
{

//
// Represents one player in the game
//

class Player
{
public:
  Player(PlayerSlot& slot, LPCTSTR name, const CChips& chips);
  virtual ~Player();

  static void SetLabelAlpha(BYTE r, BYTE g, BYTE b, BYTE a);
  static void SetTextColor(BYTE r, BYTE g, BYTE b);
  static void SetTextDark(BYTE r, BYTE g, BYTE b);
  static void SetBumpText(BOOL);
  static void SetEdgeLight(BYTE r, BYTE g, BYTE b);
  static void SetEdgeDark(BYTE r, BYTE g, BYTE b);
  static ULONG GetLabelBkgnd() { return make_rgba(lr_, lg_, lb_, la_); }

  // Player painting & hit testing
  virtual BOOL draw(CDC* pDC, DWORD flags) const;
  virtual BOOL drawImage(CDC* pDC, const CPoint& origo, DWORD flags) const;
  virtual BOOL drawChips(CDC* pDC);
  virtual BOOL drawLabels(CDC*, const CPoint& origo, DWORD flags) const;
  virtual BOOL drawCards(CDC*, const CPoint& origo, DWORD flags) const;
  virtual CRect getCardPaintArea(int num) const;
  virtual CRect getCardHitArea(int num) const;
  virtual CRect getChipsArea(int num = 0) const;
  virtual CSize getSize() const { return size_; }
  virtual CRect getArea(bool deflatebyOne = false) const;
  virtual CString getTooltipText(const CPoint& pos) const;
  virtual BOOL resetTooltip(const CPoint& pos) const;
  BOOL hitTest(int cardNum, const CPoint& pt) const;

  // Player state
  void setState(int s) { state_ = s; }
  int  getState() const { return state_; }
  void setFolded(BOOL b);
  void setSitIn(BOOL b);
  void setSitOut(BOOL b) { setSitIn(!b); }
  BOOL hasFolded() const { return (state_ == Base::Fold); }
  BOOL isSittingIn() const { return (state_ == Base::SitIn); }
  BOOL isSittingOut() const { return (state_ == Base::SitOut); }
  BOOL isInGame() const { return (state_ == Base::SitIn ||
                                  state_ == Base::AllIn); }

  virtual CChips getChips() const;
  virtual void setChips(const CChips&);
  virtual CChips getBetChips() const;
  virtual void setBetChips(const CChips&);
  virtual void invalidateChips();
  CString getName() const { return name_; }
  void setName(const CString& s);
  CString getHomeCity() const { return city_; }
  void setHomeCity(LPCTSTR city) { city_ = city; }
  CPoint getPos() const { return slot_.pos_; }
  CRect getActionTextRect() const;
  void setActionText(LPCTSTR s, BOOL invalidate = TRUE);
  CString getActionText() const { return actionText_; }
  BOOL actionTextIsEmpty() const { return actionText_.IsEmpty(); }
  CRect getChipsCompartment() const;
  void computeArea(CDC*) const;

  enum {
    NoSelection = -1
  };
  int firstSelectedCard() const;
  int lastSelectedCard() const;  

  enum AddPos {
    AddFront    = 1,
    AddBack     = 2
  };

  enum {
    AllCards         = -2,
    LastCard         = -1,
    CardSelectOffset = 13
  };

  virtual void addCard(const Card&, AddPos = AddBack);
  virtual BOOL popCard(Card&);
  virtual BOOL removeCard(int index, Card&);
  virtual BOOL setCard(int index, const Card&);
  virtual int  numCards() const;
  virtual void empty();
  virtual void setShowAll(BOOL b) { /* local player shows all cards */ }  
  virtual void mouseClick(const CPoint&);
  virtual CPoint getCardPos(int index = LastCard) const;
  virtual CPoint getChipStartPos() const;
  virtual CPoint getChipBetPos() const;
  CPoint getClosedCardPos(int index = LastCard) const;
  virtual CRect getTextArea() const;  
          BOOL getCard(int index, Card&);

  void copyDeck(std::vector<Card>&);
  void setDeck(const std::vector<Card>&);

  bool isZombie() const { return (state_ & Base::Zombie) != 0; }

protected:
  virtual CPoint makeCardPos(int index, int yoffset) const;
  BOOL   drawBounds() const; 

  COLORREF colorLight() const;
  COLORREF colorDark() const;

protected:
  CString name_, displayName_;
  CString city_;
  bool    zombie_;
  DWORD   state_;
  CChips  chips_;
  CChips  betChips_;

  // A hand consists of CardEntries; a CardEntry
  // is the card + selection state
  struct CardEntry
  {
    Card card_;
    BOOL selected_;

    CardEntry()
      : selected_(FALSE)
    {}

    CardEntry(const Card& card)
      : card_(card), selected_(FALSE)
    {}

    CardEntry(const CardEntry& ce)
      : card_(ce.card_), selected_(ce.selected_)
    {}

    CardEntry& operator = (const CardEntry& rhs)
    {
      if (this != &rhs)
      {
        card_ = rhs.card_;
        selected_ = rhs.selected_;
      }
      return *this;
    }
  };

  typedef std::list<CardEntry> Hand;
  Hand cards_;

  static CSize DefaultSize_; 
  CSize        size_;
  CSize        labelSize_;
  CSize        chipSize_;

  CString      actionText_;

public:
  // Label color & alpha values
  static BYTE lr_, lg_, lb_, la_;

  // Label edge colors:
  static BYTE edge_lightr_, edge_lightg_, edge_lightb_;
  static BYTE edge_darkr_, edge_darkg_, edge_darkb_;

  // Text color
  static BYTE textr_, textg_, textb_;
  static BYTE text_backr_, text_backg_, text_backb_;
  static BOOL bump_text_;


protected:
  bool        drawBackside_;
  PlayerSlot& slot_;
};

} // Base

#endif