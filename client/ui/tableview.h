#pragma once
#ifndef __psi_tableview_h__
#define __psi_tableview_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CTableView
  Created  : 25.10.1999

  OVERVIEW : Implements the game pane view.

 ****************************************************************************/

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <vector>
#include "common/chips.h"

/////////////////////////////////////////////////////////////////////////////
// CTableView view

namespace Base { class Player; class JPGImage; }
using Base::Player;


#include "base/playerslot.h"


#define DEFAULT_LOW 2
#define DEFAULT_HI 4

class CTableView : public CView
{
public:
  CTableView(const CString& title, int lolimit, int hilimit);
protected:
  DECLARE_DYNAMIC(CTableView)

// Attributes
public:

// Operations
public:
  void loadBackground(CString path, bool bailOutOnFail = true);
  CSize getImageSize() const;

  void     addPlayer(LPCTSTR name, const CChips& chips, int slot);
  void     addPlayer(Player* p) { players_.push_back(PlayerSlot(p)); }
  void	   updatePlayer(LPCTSTR name, const CChips& chips, int slot);
  void     removePlayer(int slot);
  void     setState(int slot, int state);
  void     setPlayerPos(int slot, const CPoint& p);
  void     setPlayerLabelPos(int slot, const CPoint& p);
  void     setPlayerOpenCardPos(int slot, const CPoint& p);
  void     setPlayerBacksidePos(int slot, const CPoint& p);
  void     setPlayerChipPos(int slot, const CPoint& p, int anchor);
  void     setPlayerTurnindicatorDir(int slot, PlayerSlot::TurnIndicatorDirection);
  void     invalidatePlayerArea(int slot, bool includeButton) const;
  PlayerSlot::TurnIndicatorDirection playerTurnindicatorDir(int slot) const;
  static   void SetAnnouncementRect(const CRect&);

  Player*  getLocalPlayer() const { return pLocalPlayer_; }
  void     setLocalPlayer(Player* p) { pLocalPlayer_ = p; }
  Player*  getHouse() const { return house_.pPlayer_; }
  void     setHouse(Player* p) { house_.pPlayer_ = p; }
  Player*  getCommunity() const { return community_.pPlayer_; }
  void     setCommunity(Player* p) { community_.pPlayer_ = p; }

  int     numPlayers(DWORD flags = 0) const;
  Player* getPlayer(int playerNum) const;
  Player* getPlayer(const CString& name) const;
  Player* hitTest(const CPoint&) const;
  Player* hitTestLabel(const CPoint&) const;
  BOOL    canBuyIn() const;

  void    setCaption();
  void    resetCaption();
  void    reloadConfiguration();
  void    resetCards(bool invalidate = false);
  CChips  resetPot(bool invalidate = true);
  void    shuffle();

  CChips  lolimit() const { return lolimit_; }
  CChips  hilimit() const { return hilimit_; }
  void    lolimit(const CChips& l) { lolimit_ = l; }
  void    hilimit(const CChips& l) { hilimit_ = l; }
  void    setIsHiLo(bool b) { isHiLo_ = b; }
  bool    getIsHiLo() const { return isHiLo_; }
  void    setTableNumber(int n) { tableNumber_ = n; }
  int     getTableNumber() const { return tableNumber_; }
  CString getGameTitle() const;

  // Table maintains the current dealer position
  int     dealer() const { return dealerSlot_; }
  void    setDealer(int, bool repaint = false);
  int     lastPlayer() const;
  bool    isLast(int) const;
  bool    isLocalPlayer(int) const;
  int     getNextPlayer(int) const;
  int     getNextPlayerInGame(int) const;
  int     getPreviousPlayer(int) const;
  int     getLeftOfBigBlind() const;
  int     getSmallBlind() const;
  int     getBigBlind() const;
  int     getLocalPlayerIndex() const;
  void    announce(const CString& s);
  void    setGameNumber(DWORD, BOOL repaint = TRUE);
  DWORD   getGameNumber() const;
  DWORD   getPrevGameNumber() const;
  void    showWinner(int winnerSlot, UINT msecs);
  void    recreateLeaveTableButton(UINT id);
  void    endBettingRound(bool lastRound = false);

  enum {
    Margin = 8,
    MaxPlayers = 10
  };
  void recalcLayout();
  void getMinMaxInfo(MINMAXINFO*);

  static CTableView* Instance() { return pInstance_; }

  void setOnlineStatus(BOOL isOnline);

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CTableView)
public:
  virtual void OnInitialUpdate();
  virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
  virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
  virtual void OnDraw(CDC* pDC);      // overridden to draw this view
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);  
  //}}AFX_VIRTUAL

// Implementation
protected:
  virtual ~CTableView();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

  void drawBackground(CDC*, const CSize&, bool);
  void drawIndicators(CDC*);
  void drawGameNumber(CDC*);
  void drawAnnouncement(CDC* pDC);
  void getIndicatorArea(CRect* dealer,
                        CRect* smallBlind,
                        CRect* bigBlind);

  void editComment(Player*);
  CString getComment(Player*);

  // Generated message map functions
protected:
  //{{AFX_MSG(CTableView)
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnViewBackground();
  afx_msg void OnReloadConfiguration();
  afx_msg void OnUpdateReloadConfiguration(CCmdUI* pCmdUI);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
  afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
// DEBUG UI FUNCTIONS:
/*
  afx_msg void OnFakeBlindPost();
  afx_msg void OnUpdateFakeBlindPost(CCmdUI* pCmdUI);
  afx_msg void OnFakePducommunitycard();
  afx_msg void OnUpdateFakePducommunitycard(CCmdUI* pCmdUI);
  afx_msg void OnFakePdudealcards();
  afx_msg void OnUpdateFakePdudealcards(CCmdUI* pCmdUI);
  afx_msg void OnFakePduerror();
  afx_msg void OnUpdateFakePduerror(CCmdUI* pCmdUI);
  afx_msg void OnFakePduplayer();
  afx_msg void OnUpdateFakePduplayer(CCmdUI* pCmdUI);
  afx_msg void OnFakePdushowdown();
  afx_msg void OnUpdateFakePdushowdown(CCmdUI* pCmdUI);
  afx_msg void OnFakePdubuyinquery();
  afx_msg void OnUpdateFakePdubuyinquery(CCmdUI* pCmdUI);
  afx_msg void OnFakePduactionrequest();
  afx_msg void OnUpdateFakePduactionrequest(CCmdUI* pCmdUI);
  afx_msg void OnFakePduannounce();
  afx_msg void OnUpdateFakePduannounce(CCmdUI* pCmdUI);
  afx_msg void OnFakePdulogoutconfirm();
  afx_msg void OnUpdateFakePdulogoutconfirm(CCmdUI* pCmdUI);
  afx_msg void OnFakePdutableaccept();
  afx_msg void OnFakePdubutton();
  afx_msg void OnUpdateFakePdubutton(CCmdUI* pCmdUI);
  afx_msg void OnSplitpot();
  afx_msg void OnTimer(UINT nIDEvent);
  afx_msg void OnFakefold();
*/
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

private:
  void clearActionCompartments();
  void moveBetChipsToPot();
  void clearBetChips();

  CString title_;
  CString configuration_;

  // Current high & low limits
  CChips  lolimit_, hilimit_;
  bool    isHiLo_;
  int     tableNumber_;

  // Dealer slot position - rotates after every hand
  int     dealerSlot_;
  int     prevSmallBlind_, prevBigBlind_;

  DWORD   gameNumber_, prevGameNumber_;

  CString announcement_;// Announcement text rendered on the play surface
  CButton btnHide_;     // Button for hiding announcement
  CButton btnLeave_;    // Leave Table button 

  // List of players
  Players players_;
  Player* pLocalPlayer_;
  PlayerSlot house_;
  PlayerSlot community_;

  static CTableView* pInstance_;

// Support for player tooltips
  CToolTipCtrl toolTip_;
  Player*      pPlayerHit_;

  Base::JPGImage* pImage_;
};


inline DWORD CTableView::getGameNumber() const
{
  return gameNumber_;
}

inline DWORD CTableView::getPrevGameNumber() const
{
  return prevGameNumber_;
}


//{{AFX_INSERT_LOCATION}}

#endif 
