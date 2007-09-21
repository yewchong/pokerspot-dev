/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CTableView
  Created  : 25.10.1999

  OVERVIEW : Implements the game table view.

 ****************************************************************************/

#include "stdafx.h"
#include "ui/tableview.h"
#include "ui/global.h"
#include "ui/actionbar.h"
#include "ui/cmdid.h"
#include "ui/resource.h"
#include "ui/animatewinner.h"
#include "ui/ownerdrawmenu.h"
#include "ui/commentdialog.h"
#include "base/cards.h"
#include "base/graphic_chips.h"
#include "base/card.h"
#include "base/gamelogic.h"
#include "base/player.h"
#include "base/draw.h"
#include "base/jpgimage.h"
#include "base/layout.h"
#include "base/loopguard.h"
#ifdef DRAW_ANNOUNCE_TEXT_
#include "base/announcetext.h"
#endif
#include "base/misc.h"
// XXX fixme: should not depend on tem!
#include "tem/remoteplayer.h"
#include "tem/house.h"
#include "tem/community.h"
#include "tem/playerindex.h"
#include "network/server.h"
#include "network/pdudealcards.h"
#include "network/pduactionecho.h"  
#include "network/pducommunitycard.h"
#include "network/pduplayer.h"  
#include "network/pduplayerinfo.h"  
#include "network/pdubutton.h"  
#include "network/pdushowdown.h"
#include "network/pdusetup.h"
#include "network/pduerror.h"
#include "network/pdublindrequest.h"
#include "network/pduactionrequest.h"
#include "network/pduannounce.h"
#include "network/pduallin.h"
#include "common/stringoutput.h"
#include <algorithm>
#include <iterator>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Define this to allow commenting players
#define PLAYER_COMMENTS_ 1

using Base::Card;
using Base::GameLogic;

//
// Game must export these functions. These functions
// implement game specific logic.
// XXX TODO: wrap these into an interface!
//
extern Base::GameState* CreateStartState();
extern void             InitGame(CTableView*);
extern void             RecalcLayout(CTableView*);


namespace
{
  //
  // Module globals
  //
  LPCTSTR g_szFilter     = _T("Layout Files (*.txt)|*.txt||");  
  LPCTSTR g_szSmallBlind = _T("Small Blind");
  LPCTSTR g_szBigBlind   = _T("Big Blind");
  LPCTSTR g_szLeaveTable = _T("Leave Table");
  LPCTSTR g_szCancelLogout = _T("Cancel Logout");

  Base::TransparentBitmap g_DealerButton;
#ifdef DRAW_ANNOUNCE_TEXT_
  // Announce text show in chat window only
  Base::TransparentBitmap g_AnnounceCorner;
  CRect g_AnnounceRect(250, 55, 400, 150);
  CRect g_AnnounceTextArea(0, 0, 0, 0);
  const int HideBtnWidth = 40;
  const int HideBtnHeight = 18;
  BOOL g_bDrawAnnounceOrnaments = FALSE;
#endif
  CFont g_BtnFont;

  BOOL PlayerIsInGame(Player* pCommunity, int player)
  {
    return static_cast<Community*>(pCommunity)->isPlayerEligible(0, player);
  }
}


/////////////////////////////////////////////////////////////////////////////
// CTableView

IMPLEMENT_DYNAMIC(CTableView, CView)

CTableView* CTableView::pInstance_ = 0;


CTableView::CTableView(const CString& title, int lolimit, int hilimit)
  :
  title_         (title),
  lolimit_       (lolimit),
  hilimit_       (hilimit),
  tableNumber_   (0),
  isHiLo_        (false),
  dealerSlot_    (-1),
  prevBigBlind_  (-1),
  prevSmallBlind_(-1),
  gameNumber_    (0),
  prevGameNumber_(0),
  pLocalPlayer_  (0),
  players_       (MaxPlayers),
  pPlayerHit_    (0),
  pImage_        (0)
{
  pInstance_ = this; 
  BOOL b = g_DealerButton.LoadBitmap(IDB_DEALERIMAGE, IDB_DEALERMASK);
  ASSERT(b);

  srand((unsigned)time(NULL));
}


CTableView::~CTableView()
{
  delete pImage_;
  pImage_ = 0;

  pInstance_ = 0;

  delete house_.pPlayer_;
  house_.pPlayer_ = 0;
  delete community_.pPlayer_;
  community_.pPlayer_ = 0;

  for (int i = 0; i < MaxPlayers; i++)
  {
    delete players_[i].pPlayer_;
    players_[i].pPlayer_ = 0;
  }
  players_.clear();

  // Make sure GameLogic does not have states
  // referring to this window
  GameLogic::Instance().setNextState(0);
  GameLogic::Instance().resetStates();

}


BEGIN_MESSAGE_MAP(CTableView, CView)
  //{{AFX_MSG_MAP(CTableView)
  ON_WM_ERASEBKGND()
  ON_WM_LBUTTONUP()
  ON_WM_SIZE()
  ON_WM_KEYDOWN()
  ON_COMMAND(ID_VIEW_BACKGROUND, OnViewBackground)
  ON_COMMAND(ID_RELOAD_CONFIGURATION, OnReloadConfiguration)
  ON_UPDATE_COMMAND_UI(ID_RELOAD_CONFIGURATION, OnUpdateReloadConfiguration)
  ON_WM_MOUSEMOVE()
  ON_WM_RBUTTONUP()
  ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNeedText)
/*
  ON_COMMAND(ID_FAKE_PDUBLINDPOST, OnFakeBlindPost)
  ON_UPDATE_COMMAND_UI(ID_FAKE_PDUBLINDPOST, OnUpdateFakeBlindPost)
  ON_COMMAND(ID_FAKE_PDUCOMMUNITYCARD, OnFakePducommunitycard)
  ON_UPDATE_COMMAND_UI(ID_FAKE_PDUCOMMUNITYCARD, OnUpdateFakePducommunitycard)
  ON_COMMAND(ID_FAKE_PDUDEALCARDS, OnFakePdudealcards)
  ON_UPDATE_COMMAND_UI(ID_FAKE_PDUDEALCARDS, OnUpdateFakePdudealcards)
  ON_COMMAND(ID_FAKE_PDUERROR, OnFakePduerror)
  ON_UPDATE_COMMAND_UI(ID_FAKE_PDUERROR, OnUpdateFakePduerror)
  ON_COMMAND(ID_FAKE_PDUPLAYER, OnFakePduplayer)
  ON_UPDATE_COMMAND_UI(ID_FAKE_PDUPLAYER, OnUpdateFakePduplayer)
  ON_COMMAND(ID_FAKE_PDUSHOWDOWN, OnFakePdushowdown)
  ON_UPDATE_COMMAND_UI(ID_FAKE_PDUSHOWDOWN, OnUpdateFakePdushowdown)
  ON_COMMAND(ID_FAKE_PDUBUYINQUERY, OnFakePdubuyinquery)
  ON_UPDATE_COMMAND_UI(ID_FAKE_PDUBUYINQUERY, OnUpdateFakePdubuyinquery)
  ON_COMMAND(ID_FAKE_PDUACTIONREQUEST, OnFakePduactionrequest)
  ON_UPDATE_COMMAND_UI(ID_FAKE_PDUACTIONREQUEST, OnUpdateFakePduactionrequest)
  ON_COMMAND(ID_FAKE_PDUANNOUNCE, OnFakePduannounce)
  ON_UPDATE_COMMAND_UI(ID_FAKE_PDUANNOUNCE, OnUpdateFakePduannounce)
  ON_COMMAND(ID_FAKE_PDULOGOUTCONFIRM, OnFakePdulogoutconfirm)
  ON_UPDATE_COMMAND_UI(ID_FAKE_PDULOGOUTCONFIRM, OnUpdateFakePdulogoutconfirm)
  ON_COMMAND(ID_FAKE_PDUTABLEACCEPT, OnFakePdutableaccept)
  ON_COMMAND(ID_FAKE_PDUBUTTON, OnFakePdubutton)
  ON_UPDATE_COMMAND_UI(ID_FAKE_PDUBUTTON, OnUpdateFakePdubutton)
  ON_COMMAND(ID_SPLITPOT, OnSplitpot)
  ON_WM_TIMER()
  ON_COMMAND(ID_FAKEFOLD, OnFakefold)
*/
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTableView drawing


void CTableView::OnDraw(CDC* pDC)
{
  int i = 0;
  CFont* pOldFont = pDC->SelectObject(Global::GetFont());
  pDC->SetBkMode(TRANSPARENT);

  bool isOnline = (Global::TableServer() != NULL);

  // allow debugging
  if (!isOnline)
    isOnline = (players_[0].pPlayer_ != NULL);

  if (isOnline)
  {
    // Draw House and Community
    Player* house = house_.pPlayer_;
    Player* community = community_.pPlayer_;

    if (house && pDC->RectVisible(&house->getArea()))
      house->draw(pDC, 0);
    if (community && pDC->RectVisible(&community->getArea()))
      community->draw(pDC, 0);

    // Draw the players
    for (i = 0; i < MaxPlayers; i++)
    {
      Player* player = players_[i].pPlayer_;
      if (player && pDC->RectVisible(&player->getArea()))
        player->draw(pDC, 0);
    }

    // Bet chips are drawn separately
    for (i = 0; i < MaxPlayers; i++)
    {
      Player* player = players_[i].pPlayer_;
      if (player && pDC->RectVisible(&player->getChipsArea()))
        player->drawChips(pDC);
    }

    // Community draws the pot chips
    if (community && pDC->RectVisible(&community->getChipsArea()))
      community->drawChips(pDC);

    // Draw positional indicators & other
    // additional stuff
    drawIndicators(pDC);
    drawAnnouncement(pDC);
    drawGameNumber(pDC);
  }

  // Allow the state to draw
  GameLogic::Instance().draw(pDC);

  pDC->SelectObject(pOldFont);
}

/////////////////////////////////////////////////////////////////////////////
// CTableView diagnostics

#ifdef _DEBUG
void CTableView::AssertValid() const
{
	CView::AssertValid();
}

void CTableView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTableView message handlers

namespace Base
{
  extern BOOL DrawBitmap(CDC& dc, CBitmap* pBitmap,
	                 const CRect* rcDst, const CRect* rcSrc);
}


BOOL CTableView::OnEraseBkgnd(CDC* pDC) 
{
  CRect r;
  GetClientRect(&r);

  if (!pImage_ || (pImage_->getSize().cx == 0 || pImage_->getSize().cy == 0))
  {
    CBrush br;
    br.CreateSolidBrush(RGB(0, 112, 0));
    pDC->FillRect(&r, &br);
    return TRUE;
  }

  if (pImage_)
  { 
    CSize imageSize = pImage_->getSize();
    bool isOffline = (Global::TableServer() == NULL);

    // for debugging purposes
    if (isOffline)
      isOffline = (numPlayers() == 0);

    if (isOffline)
    { // Paint image and hatch pattern to offscreen
      // buffer, then blast the complete image to screen

      CBitmap bmp;
      bmp.CreateCompatibleBitmap(pDC, imageSize.cx, imageSize.cy);
      CDC memDC;
      memDC.CreateCompatibleDC(pDC);

      CBitmap* pOldBmp = memDC.SelectObject(&bmp);

      drawBackground(&memDC, imageSize, TRUE);
      pDC->BitBlt(0, 0, imageSize.cx, imageSize.cy,
                  &memDC, 0, 0, SRCCOPY);

      memDC.SelectObject(pOldBmp);
    }
    else
    {
      drawBackground(pDC, imageSize, FALSE);
    }

    // Fill areas outside image
    CRect rect;
    GetClientRect(&rect);
    if (rect.Width() > imageSize.cx || rect.Height() > imageSize.cy)
    {
      CBrush br;
      COLORREF c = RGB(0, 112, 0);
      br.CreateSolidBrush(c);
      CRect r2(rect);
      r2.left = rect.left + imageSize.cx;
      pDC->FillRect(&r2, &br);
      r2.right = r2.left;
      r2.left = rect.left;
      r2.top = rect.top + imageSize.cy;
      pDC->FillRect(&r2, &br);
    }
  }      

  return TRUE;
}


void CTableView::drawBackground(CDC* pDC, const CSize& size, bool offline)
{
  // Fill background with image
  if (pImage_)
  {
    /*
    CRect r;
    GetClientRect(&r);
    pImage_->drawImage(pDC->GetSafeHdc(), r);
    */
    pImage_->drawImage(pDC->GetSafeHdc());
  }

  if (offline)
  {
    CRect r(0, 0, size.cx, size.cy);
    int left = 0, top = 0;
    int width = 8, height = 8;

    CDC memDC;
    if (memDC.CreateCompatibleDC(pDC))
    {
      // Create a hatched bit pattern.
      WORD HatchBits[8] = { 0x11, 0x22, 0x44, 0x88, 0x11, 0x22, 0x44, 0x88 };
      // Use the bit pattern to create a bitmap.CBitmap bm;
      CBitmap bm;
      bm.CreateBitmap(8,8,1,1, HatchBits);// Create a pattern brush from the bitmap.
      CBitmap* pOldBmp = memDC.SelectObject(&bm);

      while (top < size.cy)
      {
        while (left < size.cx)
        {
          if (left + width > size.cx)
            width = left + width - size.cx;
          if (top + height > size.cy)
            height = top + height - size.cy;

          pDC->BitBlt(left, top, width, height, &memDC, 0, 0, SRCAND);
          left += 8;
        }
        top += 8;
        left = 0;
        width = height = 8;
      }
    }
  }
}


void CTableView::OnLButtonUp(UINT nFlags, CPoint point) 
{
  Player* pHit = hitTest(point);
  if (pHit)
    pHit->mouseClick(point);

  CView::OnLButtonUp(nFlags, point);
}


//
// Override OnInitialUpdate to
// - set the main menu
// - call game entry point functions.
// - load default background
//
void CTableView::OnInitialUpdate() 
{
  CView::OnInitialUpdate();

  CRect r;
  GetClientRect(&r);
  if (toolTip_.Create(this, TTS_ALWAYSTIP) && toolTip_.AddTool(this))
  {
    toolTip_.SendMessage(TTM_SETMAXTIPWIDTH, 0, SHRT_MAX);
    toolTip_.SendMessage(TTM_SETDELAYTIME, TTDT_AUTOPOP, SHRT_MAX);
    // Pop up immediately as mouse moves over players
    toolTip_.SendMessage(TTM_SETDELAYTIME, TTDT_INITIAL, 0);
    toolTip_.SendMessage(TTM_SETDELAYTIME, TTDT_RESHOW, 0);
    toolTip_.SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
    toolTip_.AddTool(&btnLeave_, LPSTR_TEXTCALLBACK);
  }
  else
  {
    TRACE0("CTableView::OnInitialUpdate: Error in creating the Tooltip\n");
  }

#define MAIN_MENU_ 
#ifdef MAIN_MENU_
  CWnd* pMainFrame = AfxGetMainWnd();
  if (pMainFrame)
  { // Set the Table View menu
    CMenu* pOldMenu = pMainFrame->GetMenu();
    delete pOldMenu;
    CMenu* pNewMenu = new COwnerDrawMenu();
    if (pNewMenu)
    {
      pNewMenu->LoadMenu(IDM_TABLEVIEW);
      pNewMenu->ModifyMenu(0, MF_BYPOSITION|MF_OWNERDRAW);
      //pNewMenu->ModifyMenu(0, MF_BYPOSITION, );
    }
    pMainFrame->SetMenu(pNewMenu);
  }
#endif

  // House is at index PI_House
  setHouse(new House(house_));

  // Community is at index PI_Community  
  setCommunity(new Community(community_)); 

  // Load default background, ignore errors
  if (Global::GetGameType() == GT_Holdem)
    Base::LoadLayout("holdem.conf", this);
  else if (Global::GetGameType() == GT_SevenStud)
    Base::LoadLayout("sevenstud.conf", this);
  else if (Global::GetGameType() == GT_Omaha)
    Base::LoadLayout("omaha.conf", this);

  Chips::Instance().loadChipGraphics(NULL);

  if (CActionBar::Instance())
    CActionBar::Instance()->enableButtons(0);
}

#define _countof(array) (sizeof(array)/sizeof(array[0]))

BOOL CTableView::OnToolTipNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
  BOOL bHandledNotify = FALSE;   
  CPoint mousePos;
  VERIFY(::GetCursorPos(&mousePos));
  ScreenToClient(&mousePos);
  CRect clientRect; 
  GetClientRect(clientRect);
  
  // Make certain that the cursor is in the client rect, because the
  // mainframe also wants these messages to provide tooltips for the
  // toolbar.
  if (clientRect.PtInRect(mousePos))
  {
    TOOLTIPTEXT* pTTT = (TOOLTIPTEXT*)pNMHDR;

    // NOTE: this is static so we can give an lpsz pointer.
    // This should be ok since there'll never be 2 tooltips
    // visible at one moment.
    static CString strTip;

    Player* pP = hitTestLabel(mousePos);
    if (pP)
    { 
      strTip = pP->getTooltipText(mousePos);
#ifdef PLAYER_COMMENTS_
      CString notes = getComment(pP);
      if (notes.GetLength() > 0)
      {
        strTip += CString("\r\n") + notes;
      }
#endif
    }
    else
    { // Check if mouse inside "leave table" button
      CRect r;
      btnLeave_.GetWindowRect(&r);
      ScreenToClient(&r);
      if (r.PtInRect(mousePos))
      {
        if (!GameLogic::LeaveTable())
        {
          // XXX ELIGIBLE IN SIDE POTS?
	      if (pLocalPlayer_ && pLocalPlayer_->isInGame() &&
              numPlayers() > 1)
          {
  	        strTip = _T("Leave table after current hand");
          }
	      else
            strTip = _T("Leave table now");
        }
        else
        {
          strTip = _T("You will be logged out when the hand ends. Click this button again to cancel the logout.");
        }
      }
    }

    if (!strTip.IsEmpty())
    {
      // Pass back an lpsz ptr back to caller to
      // allow for
      // more than 80 character tip texts.

      if (pNMHDR->code == TTN_NEEDTEXTA)
      { // Ascii on Win9X
        pTTT->lpszText = (LPSTR)((LPCTSTR)strTip);
      }
      else if (pNMHDR->code == TTN_NEEDTEXTW)
      { // Wide char in WinNT - XXX NOT TESTED!!!
        // If the tooltips don't appear properly on Windows NT,
        // then these lines are probably to blaim!!!
        TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
        static wchar_t wchar_buf[256];
        _mbstowcsz(wchar_buf, strTip, sizeof(wchar_buf));
        pTTTW->lpszText = wchar_buf;
      }
    }
    else
    { 
      pTTT->szText[0] = 0;
    }
    bHandledNotify = TRUE;
  }

  return bHandledNotify;
}



BOOL CTableView::OnCommand(WPARAM wParam, LPARAM lParam) 
{
  if (!GetSafeHwnd()) return FALSE;

  BOOL rc = GameLogic::Instance().onCommand(wParam, lParam);

  if (!rc)
  { // Message was not handled - handle it here
    int cmdId = LOWORD(wParam);

    switch (cmdId)
    {
    // Eat button presses if state didn't handle
    case ID_BTN_1: 
    case ID_BTN_2:
    case ID_BTN_3:
    case ID_BTN_4:
    case ID_BTN_5:
      rc = TRUE;
      break;

    case ID_BTNHIDE:
      // Hide announce text
      announce("");
      rc = TRUE;
      break;

    case ID_LOGOUT:
      if (pLocalPlayer_ && !pLocalPlayer_->isInGame() ||
          numPlayers() < 2 || Global::IsObserver())
      { // not in game - ok to logout immediately
        GetParent()->PostMessage(WM_COMMAND,
                                 MAKEWPARAM(ID_LOGOUT, 0), 0);
      }
      else
      { // will leave at next possibility
        GameLogic::LeaveTable(TRUE); 
        AfxMessageBox(_T("You will be logged out when the current hand ends."), MB_OK|MB_ICONINFORMATION);
        recreateLeaveTableButton(ID_CANCELLOGOUT);
      }
      rc = TRUE;
      break;

    case ID_CANCELLOGOUT:
      { 
        GameLogic::LeaveTable(FALSE); 
        recreateLeaveTableButton(ID_LOGOUT);
      }
      rc = TRUE;
      break;


    case ID_EDITCOMMENT:
      {
        if (pPlayerHit_)
        {
          editComment(pPlayerHit_);
        }
        rc = TRUE;
        break;
      }
    }
  }

  if (!rc)
    rc = CView::OnCommand(wParam, lParam);

  return rc;
}


void CTableView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
  BOOL rc = GameLogic::Instance().onKey(nChar, nRepCnt, nFlags);

  if (!rc)
    CView::OnKeyDown(nChar, nRepCnt, nFlags);
}



void CTableView::OnSize(UINT nType, int cx, int cy) 
{
  CView::OnSize(nType, cx, cy);

  // if back-buffering, re-create backbuffer

  recalcLayout();
}

BOOL CTableView::Create(LPCTSTR lpszClassName,
                        LPCTSTR lpszWindowName,
                        DWORD dwStyle,
                        const RECT& rect,
                        CWnd* pParentWnd,
                        UINT nID,
                        CCreateContext* pContext) 
{
  BOOL rc = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

  if (rc)
  {
    EnableToolTips(TRUE);

    // Append table name to caption
    setCaption();

    recreateLeaveTableButton(ID_LOGOUT);

    // Create announce hide button so
    // its centered bottom of announce rect
#ifdef DRAW_ANNOUNCE_TEXT_ // ANNOUNCE TEXT NOT USED
    CRect r(0, 0, HideBtnWidth, HideBtnHeight);
    r.OffsetRect(g_AnnounceRect.left + g_AnnounceRect.Width() / 2 - r.Width() / 2,
                 g_AnnounceRect.bottom - r.Height() - 1);
    if (btnHide_.Create(_T("ok"), BS_PUSHBUTTON, r, this, ID_BTNHIDE))
    {
      static CFont f;
      f.CreateStockObject(DEFAULT_GUI_FONT);
      btnHide_.SetFont(&f);
    }
#endif
  }

  return rc;
}


/////////////////////////////////////////////////////////////////////////////
// CTableView application mfunctions


int CTableView::numPlayers(DWORD flags) const
{
  int c = 0;

  for (int i = 0; i < players_.size(); i++)
  {
    if (players_[i].pPlayer_)
    {
      if (flags == 0)
        c++;
      else if (players_[i].pPlayer_->getState() == flags)
        c++;
    }
  }

  return c;
}

//
// Draw special indicators for showing the
// dealer, the small and the big blind.
//
void CTableView::drawIndicators(CDC* pDC)
{
  if (Global::GetGameType() == GT_SevenStud)
    return; // no position indicators in 7-stud

  if (dealerSlot_ != -1 && numPlayers() > 1)
  {
    CRect rectDealer(0, 0, 0, 0),
          rectSmallBlind(0, 0, 0, 0),
          rectBigBlind(0, 0, 0, 0);

    getIndicatorArea(&rectDealer,
                     &rectSmallBlind,
                     &rectBigBlind);

    const COLORREF light = RGB(Player::textr_,
                               Player::textg_,
                               Player::textb_),
                   dark  = RGB(Player::text_backr_,
                               Player::text_backg_,
                               Player::text_backb_);

    if (!rectDealer.IsRectEmpty())
    {
      g_DealerButton.TransBlt(pDC, rectDealer.left, rectDealer.top);
    }

    pDC->SetTextColor(light);

    // Show small & big blind if more than
    // 1 player playing
    int playing = numPlayers(Base::SitIn) +
                  numPlayers(Base::Fold) +
                  numPlayers(Base::AllIn);
    if (playing > 1)
    {
      CRect tmp;
      ULONG rgba = Player::GetLabelBkgnd();
      BYTE lr = 0, lg = 0, lb = 0, la = 0;
      take_rgba(rgba, lr, lg, lb, la);
      if (!rectSmallBlind.IsRectEmpty())
      { 
        Base::DrawRectAlpha(pDC,
                            rectSmallBlind,
                            lr, lg, lb, la);
        if (Player::bump_text_)
        {
          Base::DrawBumpText(pDC,
                             g_szSmallBlind,
                             rectSmallBlind.TopLeft() + CPoint(2, 0),
                             light, dark);
        }
        else
        {
          CRect r(rectSmallBlind);
          r.OffsetRect(2, 0);
          pDC->DrawText(g_szSmallBlind, r, DT_LEFT);
        }
      }

      if (!rectBigBlind.IsRectEmpty())
      { // it is possible that big blind & dealer
        // are the same - if so, offset blind rect
        if (tmp.IntersectRect(rectBigBlind, rectDealer))
        {
          rectBigBlind.OffsetRect(rectDealer.Width() + 1, 0);
          rectBigBlind.right -= (rectDealer.Width() + 1);
        }
        Base::DrawRectAlpha(pDC,
                            rectBigBlind,
                            lr, lg, lb, la);
        if (Player::bump_text_)
        {
          Base::DrawBumpText(pDC,
                             g_szBigBlind,
                             rectBigBlind.TopLeft() + CPoint(2, 0),
                             light, dark);
        }
        else
        {
          CRect r(rectBigBlind);
          r.OffsetRect(2, 0);
          pDC->DrawText(g_szBigBlind, r, DT_LEFT);
        }
      }
    }
  }                      
}

void CTableView::getIndicatorArea(CRect* pRectDealer,
                                  CRect* pRectSM,
                                  CRect* pRectBB)
{
  if (Global::GetGameType() == GT_SevenStud ||
      dealerSlot_ == -1)
  {
    if (pRectDealer)
      pRectDealer->SetRectEmpty();
    if (pRectSM)
      pRectSM->SetRectEmpty();
    if (pRectBB)
      pRectBB->SetRectEmpty();
  }
  else
  {
    CRect r;
    int charHeight = Global::GetCharHeight();
    int ofs = -(charHeight + 1);

    Player* pSmallBlind = getPlayer(getSmallBlind());
    Player* pBigBlind = getPlayer(getBigBlind());
    Player* pDealer = getPlayer(dealer());
    CRect rectDealer(0, 0, 0, 0);

    if (pRectDealer)
    {
      ASSERT(g_DealerButton.GetSafeHandle() != NULL);

      if (pDealer)
      {
        r = pDealer->getTextArea();
        r.OffsetRect(0, -g_DealerButton.GetSize().cy);
        r.right = r.left + g_DealerButton.GetSize().cx;
        r.bottom = r.top + g_DealerButton.GetSize().cy;
        *pRectDealer = r;
        rectDealer = r;
      }
      else
        *pRectDealer = CRect(0, 0, 0, 0);
    }
  
    if (pRectSM)
    {
      if (pSmallBlind)
      {
        r = pSmallBlind->getTextArea();
        r.right -= 1;
        r.bottom = r.top + charHeight + 1;
        r.OffsetRect(0, ofs);

        // it is possible that small blind & dealer
        // are the same - if so, offset blind rect
        if (pDealer != NULL && pDealer == pSmallBlind &&
            !rectDealer.IsRectEmpty())
        {
          r.OffsetRect(rectDealer.Width() + 1, 0);
          r.right -= (rectDealer.Width() / 2 + 1);
        }

        *pRectSM = r;
      }
      else
        *pRectSM = CRect(0, 0, 0, 0);
    }
     
    if (pRectBB)
    {
      if (pBigBlind)
      {
        r = pBigBlind->getTextArea();
        r.right -= 1;
        r.bottom = r.top + charHeight + 1;
        r.OffsetRect(0, ofs);
        *pRectBB = r;
      }
      else
        *pRectBB = CRect(0, 0, 0, 0);
    }
  }
}


void CTableView::SetAnnouncementRect(const CRect& r)
{
#ifdef DRAW_ANNOUNCE_TEXT_
  g_AnnounceRect = r;
  g_AnnounceTextArea.SetRectEmpty();
#endif
}


void CTableView::drawAnnouncement(CDC* pDC)
{
#ifdef DRAW_ANNOUNCE_TEXT_
  if (announcement_.GetLength() > 0)
  {
    CFont* pOldFont = pDC->SelectObject(Global::GetBoldFont());
    int oldColor = pDC->SetTextColor(RGB(255, 255, 0));

    ULONG rgba = Player::GetLabelBkgnd();
    BYTE lr = 0, lg = 0, lb = 0, la = 0;
    take_rgba(rgba, lr, lg, lb, la);

    if (g_AnnounceTextArea.IsRectEmpty())
    { // Recalculate announce text area & reposition button
      g_AnnounceTextArea = g_AnnounceRect;
      g_AnnounceTextArea = Base::SizeAnnounceText(pDC,
                                                  announcement_,
                                                  g_AnnounceTextArea);
      // Use text height and announce rect width
      g_AnnounceTextArea.left = g_AnnounceRect.left;
      g_AnnounceTextArea.right = g_AnnounceRect.right;
      g_AnnounceTextArea.bottom += (10 +  HideBtnHeight);

      g_AnnounceTextArea.InflateRect(0, 5);

      // Position the button
      CRect rectBtn(0, 0, HideBtnWidth, HideBtnHeight);
      // Center x-wise
      CenterRect(rectBtn, g_AnnounceTextArea, DT_CENTER);
      // Move to bottom of rect
      rectBtn.OffsetRect(0, g_AnnounceTextArea.bottom - HideBtnHeight - 2);
      btnHide_.MoveWindow(&rectBtn);
    }

    Base::DrawRectAlpha(pDC, g_AnnounceTextArea, lr, lg, lb, la);
    CRect r(g_AnnounceTextArea);
    r.left += 2;
    Base::DrawAnnounceText(pDC, announcement_, r);

    pDC->SetTextColor(oldColor);
    pDC->SelectObject(pOldFont);
  }
#endif
}

// Render the game number to upper
// right corner of the client area.
void CTableView::drawGameNumber(CDC* pDC)
{
  if (gameNumber_ != 0)
  {
    CString currGame, prevGame;
    currGame.Format("Game #%ld", gameNumber_);
    prevGame.Format("Previous #%ld", prevGameNumber_);

    CRect r, size(0, 0, 0, 0);
    GetClientRect(&r);
    if (pImage_)
    { // Draw the text to the upper right corner
      // of the background image
      CSize image = pImage_->getSize();
      r.right = r.left + image.cx;
    }

    if (prevGameNumber_ != 0)
    {
      pDC->DrawText(prevGame, &size, DT_CALCRECT);
    }
    else
    {
      pDC->DrawText(currGame, &size, DT_CALCRECT);
    }

    r.left = r.right - size.Width() - 3;
    COLORREF oldcolor = pDC->SetTextColor(RGB(255, 255, 0));
    pDC->DrawText(currGame, &r, 0);

    if (prevGameNumber_ != 0)
    {
      r.OffsetRect(0, size.Height() + 2);
      pDC->DrawText(prevGame, &r, 0);
    }

    pDC->SetTextColor(oldcolor);
  }
}

//
// Move all cards to table and shuffle the deck.
//
// NOTE: this function also resets all the credits;
// eventually the server will maintain the credits
// and inform client about them.
//
void CTableView::resetCards(bool invalidate)
{
  for (int i = 0; i < MaxPlayers; i++)
  {
    Player* pPlayer = players_[i].pPlayer_;
    if (pPlayer)
    {
      if (invalidate && pPlayer->numCards() > 0)
        InvalidateRect(pPlayer->getCardPaintArea(Player::AllCards));
      pPlayer->empty();
      pPlayer->setActionText("");
      pPlayer->setFolded(FALSE);
      pPlayer->setShowAll(FALSE);
    }
  }

  // Community shows the pot - reset it
  // as well.
  Player* pPot = getPlayer(PI_Community);
  ASSERT(pPot);
  if (invalidate)
  {
    if (pPot->numCards() > 0)
      InvalidateRect(pPot->getCardPaintArea(Player::AllCards));
    InvalidateRect(pPot->getTextArea());
  }
  pPot->empty();
  pPot->setChips(0);
}


CChips CTableView::resetPot(bool invalidate)
{
  // Community player shows the pot 
  Player* pPot = getPlayer(PI_Community);
  ASSERT(pPot);
  if (invalidate)
    InvalidateRect(pPot->getTextArea());
  CChips chips = pPot->getChips();
  pPot->setChips(0);
  return chips;
}

void CTableView::shuffle()
{ 
  // XXX This functionality is not needed in the client

  // Copy deck to temporary store
  ASSERT(house_.pPlayer_);
  if (!house_.pPlayer_) return;

  // Copy house cards to temporary vector
  std::vector<Card> tmp;
  house_.pPlayer_->copyDeck(tmp);
  size_t size = tmp.size();

  // shuffle it
  std::random_shuffle(tmp.begin(), tmp.end());

  // copy shuffled deck back to house
  house_.pPlayer_->setDeck(tmp);
}


void CTableView::recalcLayout()
{
  if (GetSafeHwnd() && pImage_)
  {
    // Calc scale factor, scale layout
  }
}


void CTableView::setPlayerTurnindicatorDir(int slot,
                                           PlayerSlot::TurnIndicatorDirection dir)
{
  if (slot == PI_Community)
    community_.turnDir_ = dir;
  else if (slot == PI_House)
    house_.turnDir_ = dir;
  else if (slot >= 0 && players_.size() > slot)
    players_[slot].turnDir_ = dir;
}

PlayerSlot::TurnIndicatorDirection
CTableView::playerTurnindicatorDir(int slot) const
{
  if (slot == PI_Community)
    return community_.turnDir_;
  else if (slot == PI_House)
    return house_.turnDir_;
  else if (slot >= 0 && players_.size() > slot)
    return players_[slot].turnDir_;
  else
  {
    ASSERT(FALSE);
    return PlayerSlot::TID_Top;
  }
}


void CTableView::setPlayerPos(int slot, const CPoint& p)
{
  if (slot == PI_Community)
    community_.pos_ = p;
  else if (slot == PI_House)
    house_.pos_ = p;
  else if (slot >= 0 && players_.size() > slot)
    players_[slot].pos_ = p;
}


void CTableView::setPlayerLabelPos(int slot, const CPoint& p)
{
  if (slot == PI_Community)
    community_.labelPos_ = p;
  else if (slot == PI_House)
    house_.labelPos_ = p;
  else if (slot >= 0 && players_.size() > slot)
    players_[slot].labelPos_ = p;
}

void CTableView::setPlayerChipPos(int slot, const CPoint& p, int anchor)
{
  if (slot == PI_Community)
  {
    community_.chipPos_ = p;
    community_.chipAnchor_ = anchor;
  }
  else if (slot == PI_House)
  {
    house_.chipPos_ = p;
    house_.chipAnchor_ = anchor;
  }
  else if (slot >= 0 && players_.size() > slot)
  {
    players_[slot].chipPos_ = p;
    players_[slot].chipAnchor_ = anchor;
  }
}

void CTableView::setPlayerOpenCardPos(int slot, const CPoint& p)
{
  if (slot == PI_Community)
    community_.openCardPos_ = p;
  else if (slot == PI_House)
    house_.openCardPos_ = p;
  else if (slot >= 0 && players_.size() > slot)
    players_[slot].openCardPos_ = p;
}

void CTableView::setPlayerBacksidePos(int slot, const CPoint& p)
{
  if (slot == PI_Community)
    community_.backsidePos_ = p;
  else if (slot == PI_House)
    house_.backsidePos_ = p;
  else if (slot >= 0 && players_.size() > slot)
    players_[slot].backsidePos_ = p;
}


void CTableView::getMinMaxInfo(MINMAXINFO* pMMI)
{
  static bool is_first = true;
  if (is_first)
  {
    is_first = false;
    return;
  }

  if (pImage_)
  {
    CSize size = pImage_->getSize();

    // Limit the size of the parent frame
    // based on the size of the image
    CFrameWnd* pFrame = GetParentFrame();
    ASSERT_VALID(pFrame);
    CRect frame, client;
    pFrame->GetWindowRect(&frame);
    GetClientRect(&client);
    ClientToScreen(&client);

    if (frame.Height() < client.Height())
    {
      // the window is being restored after
      // minimize - bail out
      return;
    }

    size.cx = size.cx + (frame.Width() - client.Width());
    size.cy = size.cy + (frame.Height() - client.Height());

    //pMMI->ptMaxTrackSize.x = size.cx;
    //pMMI->ptMaxTrackSize.y = size.cy;
    // Limit min size:
    pMMI->ptMinTrackSize.x = size.cx;
    pMMI->ptMinTrackSize.y = size.cy;

    if (CActionBar::Instance() &&
        CActionBar::Instance()->isDocked())
    {
        CRect r;
        CActionBar::Instance()->GetWindowRect(&r);
        pMMI->ptMinTrackSize.y += r.Height();
    }
  }
}


Player* CTableView::getPlayer(int playerNum) const
{
  if (playerNum == PI_House)
    return house_.pPlayer_;
  else if (playerNum == PI_Community)
    return community_.pPlayer_;
  else if (playerNum == PI_LocalPlayer)
    return pLocalPlayer_;
  else if (playerNum >= 0 && playerNum < 10)
    return players_[playerNum].pPlayer_;
  else
    return 0;
}


Player* CTableView::getPlayer(const CString& n) const
{
  for (int i = 0; i < MaxPlayers; i++)
  {
    if (players_[i].pPlayer_)
      if (players_[i].pPlayer_->getName() == n)
        return players_[i].pPlayer_;
  }

  return 0;      
}

int CTableView::getLocalPlayerIndex() const
{
  if (Global::IsObserver())
    return MaxPlayers;

  for (int i = 0; i < MaxPlayers; i++)
  {
    Player* plr = players_[i].pPlayer_;

    if (isLocalPlayer(i))
    {
      break;
    }
  }

  return i;
}

Player* CTableView::hitTest(const CPoint& ptHit) const
{
  Player* pHit = 0;

  for (int i = 0; i < MaxPlayers; i++)
  {
    Player* pPlayer = players_[i].pPlayer_;

    if (pPlayer && pPlayer->getArea().PtInRect(ptHit))
    {
      pHit = pPlayer;
      break;
    }
  }

  return pHit;
}


Player* CTableView::hitTestLabel(const CPoint& ptHit) const
{
  Player* pHit = 0;

  for (int i = 0; i < MaxPlayers; i++)
  {
    Player* pPlayer = players_[i].pPlayer_;

    if (pPlayer)
    {
      if (pPlayer->getTextArea().PtInRect(ptHit) ||
          pPlayer->getChipsArea().PtInRect(ptHit))
      {
        pHit = pPlayer;
        break;
      }
    }
  }

  if (!pHit)
  { // hit test community (ie pot) as well
    if (getCommunity()->getTextArea().PtInRect(ptHit) ||
        getCommunity()->getChipsArea().PtInRect(ptHit))
      pHit = getCommunity();
  }

  return pHit;
}


void CTableView::setDealer(int d, bool repaint)
{
  if (repaint)
  { // Invalidate current indicator areas
    CRect r1, r2, r3;
    getIndicatorArea(&r1, &r2, &r3);
    InvalidateRect(r1);
    InvalidateRect(r2);
    InvalidateRect(r3);
  }

  dealerSlot_ = d;

  if (repaint)
  { // Invalidate new indicator areas
    CRect r1, r2, r3;
    getIndicatorArea(&r1, &r2, &r3);
    InvalidateRect(r1);
    InvalidateRect(r2);
    InvalidateRect(r3);
  }
}


//
// Last player is the one before the
// first player (i.e., the dealer).
//
int CTableView::lastPlayer() const
{
  return dealer();
}

//
// Return 'true' if 'playerNum' is the
// player before the first player in this hand.
//
bool CTableView::isLast(int playerNum) const
{
  return playerNum == lastPlayer();
}


bool CTableView::isLocalPlayer(int playerNum) const
{
  if (Global::IsObserver())
  { // we're observing, there is no local player
    return false;
  }

  Player* pP = getPlayer(playerNum);
  return (pP != NULL && pP == pLocalPlayer_);
}


inline int Incr(int p)
{
  ++p;
  if (p >= CTableView::MaxPlayers)
    p = 0;
  return p;
}

inline int Decr(int p)
{
  --p;
  if (p < 0)
    p = (CTableView::MaxPlayers - 1);
  return p;
}

//
// Return next connected player.
//
int CTableView::getNextPlayer(int player) const
{
  int loopGuard = 0;

  int cur = player;
  player = Incr(player);

  while (!players_[player].pPlayer_ && player != cur)
  {
    player = Incr(player);

    if (++loopGuard > MaxPlayers)
      break;
  }

  return player;
}


//
// Return previous connected player.
//
int CTableView::getPreviousPlayer(int player) const
{
  int loopGuard = 0;
  int cur = player;
  player = Decr(player);

  while (!players_[player].pPlayer_ && player != cur)
  {
    player = Decr(player);

    if (++loopGuard > MaxPlayers)
    {
      TRACE("CTableView::getPreviousPlayer: loopGuard fired\n");
      break;
    }
  }

  return player;
}


//
// Return next player in game (sitting in and not folded).
//
int CTableView::getNextPlayerInGame(int player) const
{
  int loopGuard = 0;
  int cur = player;

  while ((player = getNextPlayer(player)) != cur)
  {
    if (players_[player].pPlayer_ &&
        players_[player].pPlayer_->isInGame())
      break;

    if (++loopGuard > MaxPlayers)
    {
      TRACE("CTableView::getNextPlayerInGame: loopGuard fired\n");
      break;
    }
  }

  return player;
}



int CTableView::getLeftOfBigBlind() const
{
  int p = dealer();      
  p = getNextPlayer(p);  // small blind
  p = getNextPlayer(p);  // big blind
  p = getNextPlayer(p);  // left of big blind
  return p;
}


// Return index of player sitting
// next to dealer
int CTableView::getSmallBlind() const
{
  CTableView* ncThis = const_cast<CTableView*>(this);

  int playing = numPlayers(Base::SitIn) +
                numPlayers(Base::Fold) +
                numPlayers(Base::AllIn);

  if (playing < 2)
    return PI_NULL;

  int sb = PI_NULL;

  // XXX if newversion!
  if (numPlayers() == 2)
  { // Heads up => small blind is dealer
    sb = dealer();
  }
  else
  {

    LoopGuard lg;
    sb = getNextPlayer(dealer());

    Player* p = getPlayer(sb);
    while (!p || p->isSittingOut())
    {
      if (!lg.visit(sb))
      {
        ncThis->prevSmallBlind_ = PI_NULL;
        return PI_NULL;
      }

      sb = getNextPlayer(sb);
      p = getPlayer(sb);
    }
  }

  if (sb != prevSmallBlind_)
  { // blind pos has changed, repaint
    invalidatePlayerArea(prevBigBlind_, TRUE);
    invalidatePlayerArea(sb, TRUE);
    ncThis->prevSmallBlind_ = sb;
  }

  return sb;
}


int CTableView::getBigBlind() const
{
  CTableView* ncThis = const_cast<CTableView*>(this);

  int playing = numPlayers(Base::SitIn) +
                numPlayers(Base::Fold) +
                numPlayers(Base::AllIn);

  if (playing < 2)
    return PI_NULL;

  LoopGuard lg;
  int sb = getSmallBlind();
  if (sb == PI_NULL)
    return PI_NULL;
  int bb = getNextPlayer(sb);

  Player* p = getPlayer(bb);
  while (!p || p->isSittingOut())
  {
    if (!lg.visit(bb))
    {
      ncThis->prevBigBlind_ = PI_NULL;
      return PI_NULL;
    }

    bb = getNextPlayer(bb);
    p = getPlayer(bb);
  }

  if (bb != prevBigBlind_)
  { // blind pos has changed, repaint
    invalidatePlayerArea(prevBigBlind_, TRUE);
    invalidatePlayerArea(bb, TRUE);
    ncThis->prevBigBlind_ = bb;
  }

  return bb;
}


void CTableView::invalidatePlayerArea(int slot, bool includeButton) const
{
  Player* pPlayer = getPlayer(slot);
  if (pPlayer)
  {
    CRect r = pPlayer->getArea();
    if (includeButton)
      r.top -= g_DealerButton.GetSize().cy;
    const_cast<CTableView*>(this)->InvalidateRect(&r);
  }
}


void CTableView::addPlayer(LPCTSTR name, const CChips& chips, int slot)
{
  ASSERT(slot >= 0 && slot <= 9);

  if (slot >= 0 && slot <= 9)
  {
    if (players_[slot].pPlayer_ == 0)
    {
      Player* pPlayer = 0;

      // Check if its THIS player
      CString s = Network::Server::Username();
      if (s.CompareNoCase(name) == 0)
      {
        pPlayer = new Player(players_[slot], name, chips);
        setLocalPlayer(pPlayer);
      }
      else
        pPlayer = new RemotePlayer(players_[slot], name, chips);
    }
    else
      updatePlayer(name, chips, slot);
  }
}


void CTableView::updatePlayer(LPCTSTR name, const CChips& chips, int slot)
{
  ASSERT(slot >= 0 && slot <= 9);

  if (slot >= 0 && slot <= 9 &&
      players_[slot].pPlayer_ != 0)
  {
    Player* pPlayer = players_[slot].pPlayer_;
    if (name != NULL && *name != '\0')
      pPlayer->setName(name);
    pPlayer->setChips(chips);
  }
}


void CTableView::removePlayer(int slot)
{
  if (slot >= 0 && slot <= 9 &&
      players_[slot].pPlayer_ != 0)
  {
    Player* pPlayer = players_[slot].pPlayer_;
    players_[slot].pPlayer_ = 0;
    if (pPlayer == pLocalPlayer_)
      pLocalPlayer_ = NULL;

    delete pPlayer;
  }
}


void CTableView::setState(int slot, int state)
{
  ASSERT(slot >= 0 && slot <= 9);

  if (slot >= 0 && slot <= 9 &&
      players_[slot].pPlayer_ != 0)
  {
    // If player in a blind position sits out, the blind
    // position(s) has to be shifted to next players
    int smallb = getSmallBlind();
    int bigb = getBigBlind();
    bool resetBlindPos = (state == Base::SitOut &&
                          slot == smallb ||
                          slot == bigb);

    if (resetBlindPos)
      setDealer(dealer(), true);

    Player* pPlayer = players_[slot].pPlayer_;
    pPlayer->setState(state);

    if (resetBlindPos)
    { // re-setting the dealer repaints new
      // blinds
      setDealer(dealer(), true);
    }
  }
}


void CTableView::OnViewBackground() 
{
  CFileDialog dlg(TRUE, "cnf", NULL,
                  OFN_FILEMUSTEXIST, g_szFilter, this);
  if (dlg.DoModal() == IDOK)
  {
    CString f = dlg.GetPathName();
    if (!(f.GetLength() > 3))
    {
      CString s;
      s.Format("The file name entered is not valid: %s", (LPCTSTR)f);
      MessageBox(s);
      return;
    }

    Base::LoadLayout(f, this);
    configuration_ = f;
  }   
}


void CTableView::loadBackground(CString path, bool bailOutOnFail)
{
  ASSERT(path.GetLength() > 3);
  if (!(path.GetLength() > 3)) return;

  Base::JPGImage* pImage = Base::JPGImage::LoadImage(path);

  if (!pImage)
  {
    if (bailOutOnFail)
    {
      CString s;
      s.Format("Cannot load background bitmap: %s", (LPCTSTR)path);
      MessageBox(s);
      return;
    }
  }

  // Save loaded image to data member
  delete pImage_;
  pImage_ = pImage;

  // Resize so the view is of same size as the background
  CSize size = pImage_->getSize();
  if (size.cx > 0 && size.cy > 0)
  {
    CFrameWnd* pFrame = GetParentFrame();
    ASSERT_VALID(pFrame);
    CRect frame, client;
    pFrame->GetWindowRect(&frame);
    GetClientRect(&client);
    ClientToScreen(&client);
    pFrame->MoveWindow(frame.left, frame.top,
                       size.cx + (frame.Width() - client.Width()),
                       size.cy + (frame.Height() - client.Height()));
  }

  Invalidate();
}


void CTableView::OnReloadConfiguration() 
{
  if (Global::GetGameType() == GT_Holdem)
    Base::LoadLayout("holdem.conf", this);
  else if (Global::GetGameType() == GT_SevenStud)
    Base::LoadLayout("sevenstud.conf", this);
  else if (Global::GetGameType() == GT_Omaha)
    Base::LoadLayout("omaha.conf", this);
}


void CTableView::OnUpdateReloadConfiguration(CCmdUI* pCmdUI) 
{
  if (configuration_.GetLength() == 0)
    configuration_ = "default.txt";
  pCmdUI->Enable(configuration_.GetLength() > 0);
}


void CTableView::setCaption()
{
  CWnd* pParentWnd = GetParent();
  if (pParentWnd)
  {
    CString caption(Global::AppName());

    CString number;
    number.Format(" %i ", tableNumber_);
    caption += number;

    caption += " - ";
    caption += getGameTitle();

    if (Global::IsObserver())
      caption += " (Observing)";

    pParentWnd->SetWindowText(caption);
  }
}

void CTableView::resetCaption()
{
  title_ = Global::GetGameName();
  setCaption();
}

void CTableView::reloadConfiguration()
{
  OnReloadConfiguration();
}


void CTableView::announce(const CString& text)
{
#ifdef DRAW_ANNOUNCE_TEXT_
  if (text != announcement_)
  {
    announcement_ = text;
    CRect r;
    r.UnionRect(g_AnnounceRect, g_AnnounceTextArea);
    g_AnnounceTextArea.SetRectEmpty();

    InvalidateRect(&r);

    if (text.GetLength() > 0)
      btnHide_.ShowWindow(SW_SHOW);
    else
      btnHide_.ShowWindow(SW_HIDE);
  }
#endif
}


void CTableView::setGameNumber(DWORD number, BOOL repaint)
{
  prevGameNumber_ = gameNumber_;
  gameNumber_ = number;

  if (repaint && gameNumber_ != 0)
  {
    CDC* pDC = GetDC();
    if (pDC)
    {
      CFont* pOldFont = pDC->SelectObject(Global::GetFont());

      CString s;
      s.Format("Game #%ld", gameNumber_);

      CRect r, size(0, 0, 0, 0);
      GetClientRect(&r);
      pDC->DrawText(s, &size, DT_CALCRECT);

      r.left = r.right - size.Width() - 3;
      r.bottom = r.top + size.Height();
      InvalidateRect(&r);

      pDC->SelectObject(pOldFont);
      ReleaseDC(pDC);
    }
  }
}


// Start the winner animation.
void CTableView::showWinner(int winner, UINT msecs)
{
  Player* p = getPlayer(winner);
  if (p)
  {
    CRect r = p->getTextArea();
    AnimateWinner::StartAnimate(this, r, msecs);
  }
}

// When betting round is complete, move
// bet chips to pot and clear player action
// compartments.
void CTableView::endBettingRound(bool lastRound)
{
  if (!lastRound)
  {
    moveBetChipsToPot();
    clearActionCompartments();
  }
  else
  {
    clearBetChips();
  }
}


// Clear bet chips from players.
void CTableView::clearBetChips()
{
  for (int i = 0; i < MaxPlayers; i++)
  {
    Player* player = getPlayer(i);
    if (player)
    { 
      player->setBetChips(0);
    }
  }
}

// Move players' bet chips to pot.
void CTableView::moveBetChipsToPot()
{
  Community* community =
    static_cast<Community*>(getPlayer(PI_Community));

  CChips chips;
  for (int i = 0; i < MaxPlayers; i++)
  {
    Player* player = getPlayer(i);
    if (player)
    { 
      chips += player->getBetChips();
      player->setBetChips(0);
    }
  }

  if (community)
  {
    community->setDrawChips(community->getDrawChips() + chips);
  }
}

// Clear action compartments of players who
// are in game or folded.
void CTableView::clearActionCompartments()
{
  for (int i = 0; i < MaxPlayers; i++)
  {
    Player* player = getPlayer(i);
    if (player)
    { 
        if ((player->isInGame() && (player->getState() != Base::AllIn)) ||
            player->hasFolded())
      {
        player->setActionText("");
      }
    }
  }
}


void CTableView::recreateLeaveTableButton(UINT id)
{
  if (!GetSafeHwnd()) return;

  CRect r(5, 5, 100, 30);
  if (btnLeave_.GetSafeHwnd())
    btnLeave_.DestroyWindow();
  if (btnLeave_.Create(id == ID_LOGOUT ? g_szLeaveTable : g_szCancelLogout,
                       WS_CHILD|WS_VISIBLE|WS_DISABLED|BS_PUSHBUTTON,
                       r, this, id))
  {
    if (!g_BtnFont.GetSafeHandle())
      g_BtnFont.CreateStockObject(DEFAULT_GUI_FONT);
    btnLeave_.SetFont(&g_BtnFont);
  }

  // Set the gamelogic state accordingly
  GameLogic::LeaveTable(id == ID_CANCELLOGOUT);
}


//#define DEBUG_UI_FUNCTIONS_ 0
#ifdef DEBUG_UI_FUNCTIONS_
void CTableView::OnFakeBlindPost() 
{
  // In reality the server generates this
  static int i = 0;
  if (i == 10)
    i = 0;
  PostMessage(WM_COMMAND,
              MAKEWPARAM(ID_PDURECEIVED, 0),
              reinterpret_cast<LPARAM>(new Network::PDUBlindRequest(
                                            "x", "x", i++, 1, 5000, 0)));
}

void CTableView::OnUpdateFakeBlindPost(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(players_[0].pPlayer_ != 0);
}

void CTableView::OnFakePducommunitycard() 
{
  static int i = 0;

  Network::PDUCommunityCard* p = 
    new Network::PDUCommunityCard("x", "x", 8 + i, min((i+1), 4), 90);

  i++;      
  if (i == 5)
    i = 0;

  // In reality this PDU would be generated by the server.
  // Fake a community cards pdu
  PostMessage(WM_COMMAND,
              MAKEWPARAM(ID_PDURECEIVED, 0),
              reinterpret_cast<LPARAM>(p));
}

void CTableView::OnUpdateFakePducommunitycard(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(players_[0].pPlayer_ != 0 &&
                 getCommunity() && getCommunity()->numCards() < 5);
}

void CTableView::OnFakePdudealcards() 
{
  if (dealerSlot_ == -1)
  { // dealer not set so set it!
    OnFakePdubutton();
  }

  static int dealer = 0;

  // Fake a deal cards pdu - in reality this PDU would be generated
  // by the server after it has received all sit-ins and blinds
  PostMessage(WM_COMMAND,
              MAKEWPARAM(ID_PDURECEIVED, 0),
              reinterpret_cast<LPARAM>(new Network::PDUDealCards(
                                            "x", "x",
                                            dealer, 9, 1, 8, 2, 90)));

  if (++dealer == 10)
    dealer = 0;
}

void CTableView::OnUpdateFakePdudealcards(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(players_[0].pPlayer_ != 0);
	
}

void CTableView::OnFakePduerror() 
{
  LPCTSTR errMsg = _T("This is the error message generated by the server.");
  // In reality the server generates this
  PostMessage(WM_COMMAND,
              MAKEWPARAM(ID_PDURECEIVED, 0),
              reinterpret_cast<LPARAM>(new Network::PDUErrorClient(
                                            "x", "x", errMsg, lstrlen(errMsg))));

}

void CTableView::OnUpdateFakePduerror(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

void CTableView::OnFakePduplayer() 
{
  static const char* names[] = 
  {
    "DON'TCALLINEEDTH", "player2", "player3", "player4", "player5",
    "This is a 22-char name", "player7", "player8", "player9", "player10"
  };

  static const char* cities[] =
  {
    "New York", "Chicago", "Las Vegas", "Mellunmaki", "New York",
    "New York", "Chicago", "Las Vegas", "Mellunmaki", "New York"
  };

  static int base = 0;

  if (players_[0].pPlayer_ == 0)
  { // Make the 4th player local
    using namespace Network;
    Server::Username(names[3]);

    PDUPlayerInfo* p = new PDUPlayerInfo();
    p->num_players_ = MaxPlayers;
    p->pPlayers_ = new PDUPlayerInfo::PlayerInfo[MaxPlayers];
    PDUPlayerInfo::PlayerInfo* pP = p->pPlayers_;

    // Fake PDUPlayerInfo's 
    for (int i = 0; i < MaxPlayers; i++)
    {
      pP->slot_ = i;
      pP->chips_ = base + i;
      pP->state_ = Base::SitIn;
      strncpy(pP->username_, names[i], PDU_STRINGSIZE);
      strncpy(pP->city_, cities[i], PDU_STRINGSIZE);
      ++pP;
    }

    SendMessage(WM_COMMAND,
                MAKEWPARAM(ID_PDURECEIVED, 0),
                reinterpret_cast<LPARAM>(p));
  }
  else
  {
    for (int i = 0; i < MaxPlayers; i++)
    {
      Player* p = getPlayer(i);
      p->setChips(base + i);
    }
  }

  base += 10;

  Invalidate();
}

void CTableView::OnUpdateFakePduplayer(CCmdUI* pCmdUI) 
{
  // Allow Player PDU if no players yet
  //pCmdUI->Enable(players_[0].pPlayer_ == 0);
}

void CTableView::OnFakePdushowdown() 
{
  AfxMessageBox("Not implemented");
/*
  CString msg;
  // In reality the server generates these
  for (int i = 0; i < MaxPlayers; i++)
  {
    if (players_[i].pPlayer_ != 0 &&
        players_[i].pPlayer_->isInGame())
    {
      if (i == 0)
        msg = "WINNER";
      else
        msg = "LOST";
    }
    SendMessage(WM_COMMAND,
                MAKEWPARAM(ID_PDURECEIVED, 0),
                reinterpret_cast<LPARAM>(new Network::PDUShowdown(
                                              "x", "x",
                                              i, i+2, 1, 2, 2,
                                              1000,
                                              msg, msg.GetLength())));
    UpdateWindow();
    Sleep(1000);
  }
*/
}

void CTableView::OnUpdateFakePdushowdown(CCmdUI* pCmdUI) 
{
  BOOL enable = players_[0].pPlayer_ != 0;

  if (enable)
    enable = players_[0].pPlayer_->numCards() == 2;

  pCmdUI->Enable(enable);
}

void CTableView::OnFakePdubuyinquery() 
{
  // In reality the server generates this
  PostMessage(WM_COMMAND,
              MAKEWPARAM(ID_PDURECEIVED, 0),
              reinterpret_cast<LPARAM>(new Network::PDUSetupBuyInQuery(
                                            "x", "x", 1000)));
}

void CTableView::OnUpdateFakePdubuyinquery(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(TRUE);
}

void CTableView::OnFakePduactionrequest() 
{ 
  static int i = 0;
  if (i == 10)
    i = 0;

  PostMessage(WM_COMMAND,
              MAKEWPARAM(ID_PDURECEIVED, 0),
              reinterpret_cast<LPARAM>(new Network::PDUActionRequest(
                                            "x", "x", i++, 0, 3, 15000, 0)));
}

void CTableView::OnUpdateFakePduactionrequest(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(players_[0].pPlayer_ != 0);
}


void CTableView::OnFakePduannounce() 
{
  static const CString g_Announce =
    _T("player1 wins 100 chips with Royal Straight Flush");
  PostMessage(WM_COMMAND,
              MAKEWPARAM(ID_PDURECEIVED, 0),
              reinterpret_cast<LPARAM>(new Network::PDUAnnounce(
                                            "x", "x", 0, g_Announce, g_Announce.GetLength())));
}

void CTableView::OnUpdateFakePduannounce(CCmdUI* pCmdUI) 
{
  // TODO: Add your command update UI handler code here
}
 
void CTableView::OnFakePdulogoutconfirm() 
{
  PostMessage(WM_COMMAND,
              MAKEWPARAM(ID_PDURECEIVED, 0),
              reinterpret_cast<LPARAM>(new Network::PDUSetupTableLogoutConfirm(
                                            "x", "x")));
}

void CTableView::OnUpdateFakePdulogoutconfirm(CCmdUI* pCmdUI) 
{
}

void CTableView::OnFakePdutableaccept() 
{
  LPCTSTR msg = _T("Welcome to the<BR><BR><CENTER><I>PokerSpot</I></CENTER><BR><BR>Texas Hold'em Poker Room!");

  PostMessage(WM_COMMAND,
              MAKEWPARAM(ID_PDURECEIVED, 0),
              reinterpret_cast<LPARAM>(new Network::PDUSetupTableAccept(
                                            "x", "x", msg, lstrlen(msg))));
}

void CTableView::OnFakePdubutton() 
{
  static int i = 0;
  if (i == 10)
    i = 0;

  PostMessage(WM_COMMAND,
              MAKEWPARAM(ID_PDURECEIVED, 0),
              reinterpret_cast<LPARAM>(new Network::PDUButton(
                                                 "x", "x", i++)));
}

void CTableView::OnUpdateFakePdubutton(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(players_[0].pPlayer_ != 0);
}

// Simulate pot splitting
void CTableView::OnSplitpot() 
{
  static int i = 0;
  if (i == 10)
    i = 0;

  PostMessage(WM_COMMAND,
              MAKEWPARAM(ID_PDURECEIVED, 0),
              reinterpret_cast<LPARAM>(new Network::PDUAllIn(
                                                 "x", "x", i++, 4)));
}

void CTableView::OnTimer(UINT nIDEvent) 
{
  // Test deal cards pdu
  OnFakePdudealcards();
}

void CTableView::OnFakefold() 
{
  // In reality the server generates this
  static int i = 0;
  if (i == 10)
    i = 0;
  PostMessage(WM_COMMAND,
              MAKEWPARAM(ID_PDURECEIVED, 0),
              reinterpret_cast<LPARAM>(new Network::PDUActionEcho(
                                            NULL, NULL, i++,
                                            Network::PDUAction::Fold,
                                            0))); 	
}

#endif // DEBUG_UI_FUNCTIONS_

BOOL CTableView::PreTranslateMessage(MSG* pMsg) 
{
  if (::IsWindow(toolTip_.m_hWnd) && pMsg->hwnd == m_hWnd)
  {
    switch(pMsg->message)
    {
      case WM_LBUTTONDOWN:    
      case WM_MOUSEMOVE:
      case WM_LBUTTONUP:    
      case WM_RBUTTONDOWN:
      case WM_MBUTTONDOWN:    
      case WM_RBUTTONUP:
      case WM_MBUTTONUP:
        toolTip_.RelayEvent(pMsg);
        break;
    }
  }
  return CView::PreTranslateMessage(pMsg);	
}

void CTableView::OnMouseMove(UINT nFlags, CPoint point) 
{
  if (::IsWindow(toolTip_.m_hWnd))
  {
    Player* pP = hitTestLabel(point);

    if (!pP || pP != pPlayerHit_ || pP->resetTooltip(point)) 
    {
      // Use Activate() to hide the tooltip.
      toolTip_.Activate(FALSE);
    }
    if (pP)
    {
      toolTip_.Activate(TRUE);
      pPlayerHit_ = pP;
    }
  }
  CView::OnMouseMove(nFlags, point);	
}


void CTableView::OnRButtonUp(UINT nFlags, CPoint point) 
{
  CMenu menu;
  CMenu* submenu = NULL;

  pPlayerHit_ = hitTestLabel(point);

  if (pPlayerHit_)
  {
    // Load the menu.
    menu.LoadMenu(IDM_POPUP);
    // Get the pop-up menu.
    submenu = menu.GetSubMenu(0);
    // Convert to screen coordinates.
    ClientToScreen(&point);
    // Post the menu.
    submenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,
                            point.x, point.y,
                            this, NULL);
  }
}


void CTableView::setOnlineStatus(BOOL isOnline)
{
  btnLeave_.EnableWindow(isOnline);
}


CSize CTableView::getImageSize() const
{
  CSize s(0, 0);
  if (pImage_)
    s = pImage_->getSize();
  return s;
}


BOOL CTableView::canBuyIn() const
{
  BOOL rc = FALSE;

  Player* p = CTableView::Instance()->getLocalPlayer();
  // Buy-in allowed only if player is sitting out
  if (p)
  {
    // Is sit-out check box checked?
    rc = (p->isSittingOut() && Base::GameLogic::SitOut());

    if (!rc)
    { // player is allowed to buy chips if he's sitting
      // out and he's out of chips even if sit-out is
      // not checked
      rc = (p->isSittingOut() && p->getChips() < lolimit());
    }
  }

  return rc;
}


CString CTableView::getGameTitle() const
{
  CStrOut title;

  title << lolimit_ << "/" << hilimit_ << ' ' << Global::GetGameName();

  if (isHiLo_)
    title << " High/Low 8 or better";

  return CString(title.str());
}

void CTableView::editComment(Player* player)
{
#ifdef PLAYER_COMMENTS_

  if (!player)
    return;

  CString s = player->getName();

  if (s.GetLength() > 0)
  {
    CCommentDialog dlg(s);
    dlg.DoModal();
  }
#endif
}

CString GetComment(const CString& username)
{
  CString comment;

#ifdef PLAYER_COMMENTS_

  CString fileName;
  fileName.Format("comment/%s.txt", username);

  TRY
  {
    CFile f;
    CFileException ex;
    if (!f.Open(fileName, 
                CFile::modeRead,
                &ex))
    {
      throw new CFileException(ex.m_cause, ex.m_lOsError, ex.m_strFileName);
    }
    char buf[1024];
    memset(buf, 0, sizeof(buf));

    UINT nread = f.Read(buf, 1024);
    if (nread > 0)
    {
      buf[nread] = 0;
      comment = buf;
    }
  }
  CATCH_ALL(e)
  {
  }
  END_CATCH_ALL

#endif

  return comment;
}


CString CTableView::getComment(Player* player)
{
  if (player)
    return GetComment(player->getName());
  else
    return CString("");
}