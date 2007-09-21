// LoungeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "loungedlg/lounge.h"
#include "loungedlg/loungedlg.h"
#include "loungedlg/loungedata.h"
#include "loungedlg/cashierdlg.h"
#include "loungedlg/tabledetailsdlg.h"
#include "loungedlg/tournamentdetailsdlg.h"
#include "loungedlg/registry.h"
#include "loungedlg/banner.h"
#include "loungedlg/floormap.h"
#include "ui/global.h"
#include "ui/cmdid.h"
#include "ui/logindlg.h"
#include "ui/splashwnd.h"
#include "ui/aboutdlg.h"
#include "ui/chatdlg.h"
#include "base/draw.h"
#include "base/misc.h"
#include "network/network.h"
#include "network/server.h"
#include "network/pduverbosetablereply.h"
#include "network/pdulounge.h"
#include "base/jpgimage.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
  LPCTSTR g_szAddToQueue = _T("Add Name");
  LPCTSTR g_szRemoveFromQueue = _T("Remove");
  LPCTSTR g_szViewByWaitingLists = _T("View by Waiting Lists");
  LPCTSTR g_szViewByTables = _T("View by Tables");
  LPCTSTR g_szRealMoneyTitle = _T("PokerSpot Lounge - Real Money");
  LPCTSTR g_szPlayMoneyTitle = _T("PokerSpot Lounge - Play Money");

  const int MaxColumnWidth = 100;
  const int MinColumnWidth = 20;

  using namespace Lounge;

  int FindColumn(const Game& game, USHORT queueNum)
  {
    int index = 0;

    // Find the queue with matching limits and
    // return its index (== column index)
    for (Queues::const_iterator it = game.queues_.begin(),
                                end = game.queues_.end();
         it != end;
         ++it)
    {
      const Queue& q = *it;
      if (q.queueNumber_ == queueNum)
      {
        return index;
      }
      ++index;
    }

    // If reached, we have a table with limits
    // that does not have corresponding queue
    ASSERT(FALSE); 
    return -1;
  }

  bool g_bInPlayMoney = false;
  bool g_bShowMotd = false;

} // namespace


bool ShowMotd()
{
    return g_bShowMotd;
}

bool InPlayMoney()
{
    return g_bInPlayMoney;
}

void SetInPlayMoney(bool b)
{
    g_bInPlayMoney = b;
}

void SetPlayMoneyTitle(bool b)
{
    SetInPlayMoney(b);
    CWnd* wnd = AfxGetMainWnd();
    if (wnd && wnd->IsKindOf(RUNTIME_CLASS(CLoungeDlg)))
    {
        if (b)
            wnd->SetWindowText(g_szPlayMoneyTitle);
        else
            wnd->SetWindowText(g_szRealMoneyTitle);
    }
}

bool GetInPlayMoney()
{
    return g_bInPlayMoney;
}

IMPLEMENT_DYNAMIC(CLoungeDlg, CDialog);

BOOL CLoungeDlg::InPlayMoney()
{
  return ::InPlayMoney();
}

CLoungeDlg::CLoungeDlg(CWnd* pParent /*=NULL*/)
  :
  CDialog       (CLoungeDlg::IDD, pParent),
/*
  bannerPos_    (0),
  bannerTimer_  (-1),
  bannerDir_    (false),
*/  
  timer_        (0),
  pBanner_      (0),
  pChatDlg_     (0),
  pDetailsDlg_  (0),
  pTournamentDlg_(0),
  pImage_       (0),
  refresh_      (TRUE),
  radioYOffset_ (0)
{
  //{{AFX_DATA_INIT(CLoungeDlg)
  //}}AFX_DATA_INIT

  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

  pImage_ = Base::JPGImage::LoadImage("lounge.jpg");
}


CLoungeDlg::~CLoungeDlg()
{
  delete pImage_;
  pImage_ = 0;
}


void CLoungeDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CLoungeDlg)
  DDX_Control(pDX, IDC_LD_TAB1, tabCtrl_);
  DDX_Control(pDX, IDC_LD_LIST1, listCtrl_);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLoungeDlg, CDialog)
  //{{AFX_MSG_MAP(CLoungeDlg)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_NOTIFY(TCN_SELCHANGE, IDC_LD_TAB1, OnSelchangeTab1)
  ON_BN_CLICKED(IDC_LD_RADIO1, OnViewByWaitList)
  ON_BN_CLICKED(IDC_LD_RADIO2, OnViewByTables)
  ON_UPDATE_COMMAND_UI(ID_LOGIN, OnUpdateLogin)
  ON_UPDATE_COMMAND_UI(ID_LOGOUT, OnUpdateLogout)
  ON_NOTIFY(NM_CLICK, IDC_LD_LIST1, OnClickList)
  ON_NOTIFY(NM_RCLICK, IDC_LD_LIST1, OnRclickList)
  ON_WM_CLOSE()
  ON_BN_CLICKED(IDC_CASHIER, OnCashier)
  ON_BN_CLICKED(IDC_LOGIN, OnLogin)
  ON_WM_CTLCOLOR()
  ON_WM_LBUTTONUP()
  ON_BN_CLICKED(IDC_BUTTON_TOURNAMENT, OnButtonTournament)
  ON_WM_TIMER()
  ON_BN_CLICKED(IDC_BUTTON_TESTLOGIN, OnButtonTestlogin)
	ON_BN_CLICKED(IDC_RADIO_REALMONEY, OnRadioRealmoney)
	ON_BN_CLICKED(IDC_RADIO_PLAYMONEY2, OnRadioPlaymoney2)
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoungeDlg message handlers

BOOL CLoungeDlg::OnInitDialog()
{
  VERIFY(radioBtnOn_.LoadBitmap(IDB_RADIO_ON, IDB_RADIOMASK));
  VERIFY(radioBtnOff_.LoadBitmap(IDB_RADIO_OFF, IDB_RADIOMASK));

  CDialog::OnInitDialog();

  CFont f;
  f.CreateStockObject(DEFAULT_GUI_FONT);
  LOGFONT lf;
  f.GetLogFont(&lf);
  if (lf.lfHeight > 0)
      lf.lfHeight += 2;
  else
      lf.lfHeight -= 2;
  font_.CreateFontIndirect(&lf);
  GetDlgItem(IDC_LD_RADIO1)->SetFont(&font_);
  GetDlgItem(IDC_LD_RADIO2)->SetFont(&font_);

  CDC dc;
  dc.CreateCompatibleDC(NULL);
  CRect r;
  GetDlgItem(IDC_LD_RADIO1)->GetWindowRect(&r);
  ScreenToClient(&r);
  rectRadio1_ = r;
  CFont* pOldFont = dc.SelectObject(&font_);

  Base::DrawBumpText(&dc, g_szViewByWaitingLists,
                     rectRadio1_, DT_CALCRECT,
                     0, 0);
  rectRadio1_.right += 16;
  radioYOffset_ = rectRadio1_.Height() - r.Height();

  GetDlgItem(IDC_LD_RADIO2)->GetWindowRect(&r);
  ScreenToClient(&r);
  rectRadio2_ = r;
  Base::DrawBumpText(&dc, g_szViewByTables,
                     rectRadio2_, DT_CALCRECT,
                     0, 0);
  rectRadio2_.right += 16;
  dc.SelectObject(pOldFont);

  GetDlgItem(IDC_LD_RADIO1)->GetWindowRect(&r);
  ScreenToClient(&r);

  GetDlgItem(IDC_LD_RADIO2)->GetWindowRect(&r);
  ScreenToClient(&r);

  // IDM_ABOUTBOX must be in the system command range.
  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu* pSysMenu = GetSystemMenu(FALSE);
  if (pSysMenu != NULL)
  {
    CString strAboutMenu;
    strAboutMenu.LoadString(IDS_ABOUTBOX);
    if (!strAboutMenu.IsEmpty())
    {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  // Set the icon for this dialog.  The framework does this automatically
  //  when the application's main window is not a dialog
  SetIcon(m_hIcon, TRUE);			// Set big icon
  SetIcon(m_hIcon, FALSE);		// Set small icon
	
  addInitialData();

  pDetailsDlg_ = new CTableDetailsDlg(this);
  if (pDetailsDlg_)
    if (!pDetailsDlg_->Create(IDD_TABLEDETAILS, this))
    {
      TRACE0("Failed to create table details window\n");
    }

  pTournamentDlg_ = new CTournamentDetailsDlg(this);
  if (pTournamentDlg_)
    if (!pTournamentDlg_->Create(IDD_TOURNAMENTDETAILS, this))
    {
      TRACE0("Failed to create tourmanet details window\n");
    }

  pChatDlg_ = new CChatDlg(this);
  if (pChatDlg_)
    if (!pChatDlg_->Create(NULL, NULL, WS_CHILD|WS_VISIBLE, CFrameWnd::rectDefault, this, IDD_CHAT))
    {
      TRACE0("Failed to create chat window\n");
    }

  restoreSettings();

  // Begin with the splash screen
  PostMessage(WM_COMMAND,
              MAKEWPARAM(ID_SHOW_SPLASH,0), 0);

  //setBannerText("Testing Testing 1 2 3...", 0);

  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLoungeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
  if ((nID & 0xFFF0) == IDM_ABOUTBOX)
  {
    CAboutDlg dlg;
    dlg.DoModal();
  }
  else
  {
    CDialog::OnSysCommand(nID, lParam);
  }
}


void CLoungeDlg::OnPaint() 
{
  CRect r2;
  GetUpdateRect(&r2);
  ClientToScreen(&r2);

  if (IsIconic())
  {
    CPaintDC dc(this); // device context for painting

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

    // Center icon in client rectangle
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // Draw the icon
    dc.DrawIcon(x, y, m_hIcon);
  }
  else
  {
    CPaintDC dc(this);

    if (pImage_)
    {
      //
      // Paint the background graphics
      //
      CRect r;
      GetClientRect(&r);
      pImage_->drawImage(dc, r);

      // Need to fill the tab control tab's backround
      // with button color because it won't paint it
      if (tabCtrl_.GetSafeHwnd())
      {
        CRect r;
        tabCtrl_.GetItemRect(0, &r);        
        CRect rectTab;
        tabCtrl_.GetWindowRect(&rectTab);
        ScreenToClient(&rectTab);
        rectTab.bottom = rectTab.top +
                           tabCtrl_.GetRowCount() * r.Height() + 5;
        dc.FillSolidRect(&rectTab, GetSysColor(COLOR_BTNFACE));
        ClientToScreen(&rectTab);
        tabCtrl_.ScreenToClient(&rectTab);
        tabCtrl_.RedrawWindow(&rectTab);
      }


      if (Base::IsMaximized(this))
      {
        // fill uncovered areas 
        CBrush br;
        COLORREF c = RGB(196, 152, 96);
        br.CreateSolidBrush(c);
        CRect r;
        GetClientRect(&r);
        CSize s = pImage_->getSize();
        CRect r2(r);
        r2.left = r.left + s.cx;
        dc.FillRect(&r2, &br);
        r2.right = r2.left;
        r2.left = r.left;
        r2.top = r.top + s.cy;
        dc.FillRect(&r2, &br);
      }
    }

    int oldMode = dc.SetBkMode(TRANSPARENT);

    //
    // Paint the alpha bump rectangle and the text
    //
    BYTE red = 70, green = 32, blue = 20, alpha = 100;
    COLORREF light = RGB(224, 159, 95);
    COLORREF dark = RGB(max(0, 0.4f * float(red)),
                        max(0, 0.4f * float(green)),
                        max(0, 0.4f * float(blue)));

    CRect r;
    GetDlgItem(IDC_LOUNGE_GROUP1)->GetWindowRect(&r);
    ScreenToClient(&r);
    Base::DrawRectAlpha(&dc, r, red, green, blue, alpha);
    Base::DrawBumpRect(&dc, r, dark, light);

    CFont* pOldFont = dc.SelectObject(&font_);

    GetDlgItem(IDC_PROMPT_USERNAME)->GetWindowRect(&r);
    ScreenToClient(&r);
    Base::DrawBumpText(&dc, "Username", r.TopLeft(), RGB(255, 255, 255), RGB(0, 0, 0));

    GetDlgItem(IDC_PROMPT_STATUS)->GetWindowRect(&r);
    ScreenToClient(&r);
    Base::DrawBumpText(&dc, "Status", r.TopLeft(), RGB(255, 255, 255), RGB(0, 0, 0));

    GetDlgItem(IDC_PROMPT_NUMTABLES)->GetWindowRect(&r);
    ScreenToClient(&r);
    Base::DrawBumpText(&dc, "# Tables", r.TopLeft(), RGB(255, 255, 255), RGB(0, 0, 0));

    GetDlgItem(IDC_PROMPT_NUMPLAYERS)->GetWindowRect(&r);
    ScreenToClient(&r);
    Base::DrawBumpText(&dc, "# Players", r.TopLeft(), RGB(255, 255, 255), RGB(0, 0, 0));

    GetDlgItem(IDC_LD_RADIO1)->GetWindowRect(&r);
    ScreenToClient(&r);
    Base::DrawBumpText(&dc, g_szViewByWaitingLists, CPoint(r.right + 2, r.top), RGB(255, 255, 255), RGB(0, 0, 0));

    GetDlgItem(IDC_LD_RADIO2)->GetWindowRect(&r);
    ScreenToClient(&r);
    Base::DrawBumpText(&dc, g_szViewByTables, CPoint(r.right + 2, r.top), RGB(255, 255, 255), RGB(0, 0, 0));

    dc.SetBkMode(oldMode);
    dc.SelectObject(pOldFont);

    //
    // Update the list control if needed
    //
    listCtrl_.GetWindowRect(&r);
    if (r2.IntersectRect(&r, &r2))
    {
      listCtrl_.Invalidate();
    }

    //
    // Paint the radio buttons
    //
    CPoint ofs(0, 2);
    if (viewByWaitingLists())
    {
      radioBtnOn_.TransBlt(&dc, rectRadio1_.TopLeft() + ofs);
      radioBtnOff_.TransBlt(&dc, rectRadio2_.TopLeft() + ofs);
    }
    else
    {
      radioBtnOn_.TransBlt(&dc, rectRadio2_.TopLeft() + ofs);
      radioBtnOff_.TransBlt(&dc, rectRadio1_.TopLeft() + ofs);
    }
  }
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLoungeDlg::OnQueryDragIcon()
{
  return (HCURSOR) m_hIcon;
}

// Add new tab to the end.
void CLoungeDlg::addTab(LPCTSTR tabtitle)
{
  tabCtrl_.InsertItem(tabCtrl_.GetItemCount(), tabtitle);
}

void CLoungeDlg::adaptTabCtrl()
{
  int rows = tabCtrl_.GetRowCount();
  static int prevRows = 1;

  if (rows != prevRows)
  {
    int diff = rows - prevRows;
    // Adapt controls to two rows
    CRect r;
    tabCtrl_.GetItemRect(0, &r);
    int offset = diff * r.Height();
    CRect rectTab;
    tabCtrl_.GetWindowRect(&rectTab);
    ScreenToClient(&rectTab);
    rectTab.bottom += offset;
    tabCtrl_.MoveWindow(&rectTab);
    CRect rectList;
    listCtrl_.GetWindowRect(&rectList);
    ScreenToClient(&rectList);
    rectList.OffsetRect(0, offset);
    listCtrl_.MoveWindow(&rectList);

    prevRows = rows;
  }
}


// Add a new column to current tab's list and
// a button for the column.
void CLoungeDlg::addColumn(LPCTSTR coltitle, int totalcolumns, BOOL inQueue)
{
  const int BtnSizeY = (1.6f * Global::GetCharHeight());
  int btnId = IDC_LD_BUTTONIDBASE;
  CRect r;
  listCtrl_.GetWindowRect(&r);
  int i = listCtrl_.GetHeaderCtrl()->GetItemCount();
  int width = float(r.Width()) / float(totalcolumns);

  // force width between min and max
  width = min(width, MaxColumnWidth);
  width = max(width, MinColumnWidth);

  listCtrl_.InsertColumn(i, coltitle, LVCFMT_LEFT, width, i);
  if (buttons_.size() < (i + 1))
  {
    CButton* pBtn = new CButton();
    if (pBtn)
    {
      CRect rectBtn(r.left, r.bottom + 5, r.left + width, r.bottom + 5 + BtnSizeY);
      ScreenToClient(&rectBtn);
      rectBtn.OffsetRect(i * width, 0);
        
      UINT style = WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON;
      if (inQueue)
        pBtn->Create(g_szRemoveFromQueue, style, rectBtn, this, btnId + i);
      else
        pBtn->Create(g_szAddToQueue, style, rectBtn, this, btnId + i);
      pBtn->SetFont(CFont::FromHandle(
                     (HFONT)::GetStockObject(DEFAULT_GUI_FONT)));

      buttons_.push_back(pBtn);
    }
  }
}


void CLoungeDlg::addItem(LPCTSTR text, int column)
{
  if (column < 0) return;

  int num_items = listCtrl_.GetItemCount();

  // Find the first free cell in this column
  for (int i = 0; i < num_items; i++)
  {
    CString s = listCtrl_.GetItemText(i, column);
    if (s.IsEmpty())
      break;
  }

  if (i == num_items)
  { // Need to add new item to list
    listCtrl_.InsertItem(i, 0, NULL);
  }

  listCtrl_.SetItemText(i, column, text);
}

void CLoungeDlg::removeItem(LPCTSTR text, int column)
{
  if (column < 0) return;

  int num_items = listCtrl_.GetItemCount();

  // Find the cell containing 'text' in this column
  for (int i = 0; i < num_items; i++)
  {
    CString s = listCtrl_.GetItemText(i, column);
    if (s == text)
      break;
  }

  // The item is removed by nudging the rest up
  for (; i < num_items; i++)
  {
    CString s = listCtrl_.GetItemText(i + 1, column);
    listCtrl_.SetItemText(i, column, s);
  }
}

CString CLoungeDlg::getSelectedTabText()
{
  char buf[256];
  TCITEM tcItem;

  tcItem.mask = TCIF_TEXT;
  tcItem.pszText = buf;
  tcItem.cchTextMax = 255;
  tabCtrl_.GetItem(tabCtrl_.GetCurSel(), &tcItem);

  return tcItem.pszText; // creates a copy
}


int GetLongestQueueLength(const char* n[])
{
  int longest = 0, l = 0;
  int j = 0;

  while (n[j] != '\0')
  {
    for (; n[j] != '\0'; j++)
    {
      l++;
    }

    j++;

    if (l > longest)
      longest = l;
    l = 0;
  }

  return longest;
}


void CLoungeDlg::addInitialData()
{
  // By default view by tables 
  GetDlgItem(IDC_LD_RADIO2)->SendMessage(BM_SETCHECK,
                                         MAKEWORD(BST_CHECKED, 0), 0L);
                                     
  int i = 0;
 
  static const char* Tabs[] = 
  { "Games" };

  CRect r;
  tabCtrl_.GetWindowRect(&r);
  int tabs = sizeof(Tabs) / sizeof(Tabs[0]);
  for (i = 0; i < tabs; i++)
  {
    tabCtrl_.InsertItem(i, Tabs[i]);
  }

  adaptTabCtrl();
  
  // Create the limit columns and a button for each column
  static const char* Lims[] = 
  { "Limits" };

  const int BtnSizeY = (1.6f * Global::GetCharHeight());
  int btnId = 1200;
  listCtrl_.GetWindowRect(&r);
  int columns = sizeof(Lims) / sizeof(Lims[0]);

  int width = float(r.Width()) / float(columns) - 1;
  // force width between min and max
  width = min(width, MaxColumnWidth);
  width = max(width, MinColumnWidth);

  CWnd* pHeader = listCtrl_.GetHeaderCtrl();
  if (listCtrl_.GetSafeHwnd() && !pHeader)
  {
      AfxMessageBox("PokerSpot Lounge needs a newer version of a system DLL (comctl32.dll) in order to run.\r\n"
                    "Please contact support@pokerspot.com for further instructions.");
      PostMessage(WM_CLOSE, 0, 0);
      return;
  }
  for (i = 0; i < columns; i++)
  {
    listCtrl_.InsertColumn(i, Lims[i], LVCFMT_LEFT, width, i);
  }

  // First create the items enough for the longest queue
  int numitems = 0;//GetLongestQueueLength(Names);
  // put at least 8 rows
  numitems = max(8, numitems);
  for (int row = 0; row < numitems; row++)
  {
    listCtrl_.InsertItem(row, 0, NULL);
  }
}


void CLoungeDlg::setStats(int players, int tables)
{
  CString s;
  s.Format("%d", players);
  GetDlgItem(IDC_LD_NUMPLAYERS)->SetWindowText(s);

  s.Format("%d", tables);
  GetDlgItem(IDC_LD_NUMTABLES)->SetWindowText(s);
}


void CLoungeDlg::disconnect()
{
  Lounge::EmptyAll();

  GetDlgItem(IDC_LD_STATUS)->SetWindowText("Offline");
  GetDlgItem(IDC_LD_NUMPLAYERS)->SetWindowText("");
  GetDlgItem(IDC_LD_NUMTABLES)->SetWindowText("");

  // Reset to view by tables
  GetDlgItem(IDC_LD_RADIO1)->SendMessage(BM_SETCHECK,
                                         MAKEWORD(BST_UNCHECKED, 0), 0L);
  GetDlgItem(IDC_LD_RADIO2)->SendMessage(BM_SETCHECK,
                                         MAKEWORD(BST_CHECKED, 0), 0L);

  tabCtrl_.DeleteAllItems();
  resetList();
  addInitialData();
  GetDlgItem(IDC_LOGIN)->SetWindowText("Login...");

  Invalidate();
}


void CLoungeDlg::addGamesTabs()
{
  using namespace Lounge;

  tabCtrl_.DeleteAllItems();

  Games& games = Lounge::GetGames();
  for (Games::const_iterator git = games.begin(),
                             gend = games.end();
       git != gend;
       ++git)
  {
    addTab(git->name_.c_str());
  }

  adaptTabCtrl();
}


void CLoungeDlg::populateList(int tabindex)
{
  if (tabCtrl_.GetItemCount() > tabindex)
  {
    char buf[256];
    TCITEM tcItem;
    tcItem.mask = TCIF_TEXT;
    tcItem.pszText = buf;
    tcItem.cchTextMax = 255;
    tabCtrl_.GetItem(tabindex, &tcItem);

    populateList(tcItem.pszText);
  }
}

// Adds queues to current tab's list.
void CLoungeDlg::populateList(LPCTSTR gamename)
{
  using namespace Lounge;
  using namespace std;

  Games& games = Lounge::GetGames();

  Games::const_iterator git = find(games.begin(),
                                   games.end(),
                                   Game(gamename));
  if (git != games.end())
  {
    const Game& game = *git;

    // Add columns (queues) to this tab (game)
    for (Queues::const_iterator it = game.queues_.begin(),
                                end = game.queues_.end();
         it != end;
         ++it)
    {
      const Queue& q = *it;
      string title = q.getTitle();
      addColumn(title.c_str(), game.queues_.size(), q.inQueue_);
    }

    if (viewByTables())
    { //
      // View by tables - add the tables
      //
      int i = 0;
      for (Queues::const_iterator qit = game.queues_.begin(),
                                  qend = game.queues_.end();
           qit != qend;
           ++qit, ++i)
      {
        const Queue& q = *qit;
        for (Tables::const_iterator it = q.tables_.begin(),
                                    end = q.tables_.end();
             it != end;
             ++it)
        {
          const Table& t = *it;
          string title = t.getTitle();
          addItem(title.c_str(), i);
        }
      }
    }
    else
    { //
      // View by waiting lists - add the players in the lists
      //
      int i = 0;
      for (Queues::const_iterator it = game.queues_.begin(),
                                  end = game.queues_.end();
         it != end;
         ++it, ++i)
      {
        const Queue& q = *it;
        for (Players::const_iterator pit = q.players_.begin(),
                                     pend = q.players_.end();
             pit != pend;
             ++pit)
        {
          addItem((*pit).name_.c_str(), i);
        }        
      }
    }
  }
}


void CLoungeDlg::resetList()
{
  listCtrl_.setSelection(-1, -1);
  listCtrl_.DeleteAllItems();

  int nColumnCount = listCtrl_.GetHeaderCtrl()->GetItemCount();
  // Delete all of the columns.
  for (int i = 0; i < nColumnCount; i++)
    listCtrl_.DeleteColumn(0);

  for (std::list<CButton*>::iterator it = buttons_.begin(),	
                                     end = buttons_.end();
       it != end;
       ++it)
  {
    delete *it;
  }
  buttons_.clear();
}


void CLoungeDlg::restoreSettings()
{
  CRect r = Registry::GetWndRect(CRect(0, 0, 0, 0));
  if (!r.IsRectEmpty())
  {
    // Move window but don't change window size
    CRect window;
    GetWindowRect(&window);
    r.right = r.left + window.Width();
    r.bottom = r.top + window.Height();

    MoveWindow(&r, TRUE);
  }
  else
  { // first launch - position right of details dlg
    if (pDetailsDlg_)
    {
      pDetailsDlg_->GetWindowRect(&r);
      CRect window;
      GetWindowRect(&window);
      window.OffsetRect(r.Width(), 0);
      MoveWindow(&window, TRUE);
    }
  }

  r = Registry::GetTableDetailsRect(CRect(0, 0, 0, 0));
  if (!r.IsRectEmpty() && pDetailsDlg_)
    pDetailsDlg_->MoveWindow(r, TRUE);

  r = Registry::GetChatRect(CRect(0, 0, 0, 0));
  if (r.IsRectEmpty() && pDetailsDlg_)
  { // By default position the chat window
    // below details window
    pDetailsDlg_->GetWindowRect(&r);
    r.OffsetRect(0, r.Height());
    r.bottom = r.top + r.Height() / 2;
  }
  if (!r.IsRectEmpty() && pChatDlg_)
    pChatDlg_->MoveWindow(&r, TRUE);   
  if (pChatDlg_)
    pChatDlg_->hideTabs();


  if (pChatDlg_)
    pChatDlg_->ShowWindow(SW_NORMAL);

  if (pDetailsDlg_)
    pDetailsDlg_->ShowWindow(SW_NORMAL);
}


void CLoungeDlg::saveSettings()
{
  CRect r;
  if (pChatDlg_)
  {
    pChatDlg_->GetWindowRect(&r);
    Registry::SetChatRect(r);
  }

  if (pDetailsDlg_)
  {
    pDetailsDlg_->GetWindowRect(&r);
    Registry::SetTableDetailsRect(r);
  }

  GetWindowRect(&r);
  Registry::SetWndRect(r);
}


BOOL CLoungeDlg::DestroyWindow() 
{
  saveSettings();

  if (pChatDlg_)
  {
    pChatDlg_->DestroyWindow();
    delete pChatDlg_;
    pChatDlg_ = 0;
  }

  if (pDetailsDlg_)
  {
    pDetailsDlg_->DestroyWindow();
    delete pDetailsDlg_;
    pDetailsDlg_ = 0;
  }

  if (pTournamentDlg_)
  {
    pTournamentDlg_->DestroyWindow();
    delete pTournamentDlg_;
    pTournamentDlg_ = 0;
  }

  for (std::list<CButton*>::iterator it = buttons_.begin(),	
                                     end = buttons_.end();
       it != end;
       ++it)
  {
    delete *it;
  }

  buttons_.clear();

  return CDialog::DestroyWindow();
}


BOOL CLoungeDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
  BOOL rc = FALSE;
  UINT cmd = LOWORD(wParam);

  if (cmd >= IDC_LD_BUTTONIDBASE &&
      cmd < IDC_LD_BUTTONIDBASE + buttons_.size())
  {
    CWnd* pBtn = GetDlgItem(cmd);
    if (pBtn)
    {
      CString s;
      pBtn->GetWindowText(s);
      if (s == g_szAddToQueue)
      {
        joinOrRemoveQueue(cmd - IDC_LD_BUTTONIDBASE, TRUE);
        pBtn->SetWindowText(g_szRemoveFromQueue);
      }
      else
      {
        joinOrRemoveQueue(cmd - IDC_LD_BUTTONIDBASE, FALSE);
        pBtn->SetWindowText(g_szAddToQueue);
      }
    }
    
    rc = TRUE;
  }
  else
  {
    switch (cmd)
    {    
    case ID_LOGIN:
      doLogin();
      rc = TRUE;
      break;

    case ID_LOGOUT:
      doLogout();
      rc = TRUE;
      break;
  
    case ID_PDURECEIVED:
      handlePDU(reinterpret_cast<Network::PDUHeader*>(lParam));
      rc = TRUE;
      break;

    case ID_SHOW_SPLASH:
    {
      rc = TRUE;
      if (!CSplashWnd::ShowSplashScreen("splash_lounge.jpg",
                                        TRUE, // post login when close
                                        TRUE, // close on timer
                                        this))
      {
        if (AfxGetMainWnd())
          AfxGetMainWnd()->PostMessage(WM_COMMAND,
                                       MAKEWPARAM(ID_LOGIN, 0),
                                       0);
      }
    }

    default:
      break;
    }
  }

  if (!rc)
    rc = CDialog::OnCommand(wParam, lParam);

  return rc;
}


void CLoungeDlg::doLogin()
{
  CLoginDlg dlg(this);
  if (dlg.DoModal() == IDOK)
  {
    GetDlgItem(IDC_LD_NAME)->SetWindowText(dlg.username_);
    GetDlgItem(IDC_LD_STATUS)->SetWindowText("Connected");
    GetDlgItem(IDC_LOGIN)->SetWindowText("Logout");
  }
}


void CLoungeDlg::OnUpdateLogin(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(Global::LoungeServer() == NULL);
}


void CLoungeDlg::doLogout()
{
  UINT action = AfxMessageBox("Are you sure you want to log out?", MB_YESNO);
  if (action == IDYES)
  {
  }
}


void CLoungeDlg::OnUpdateLogout(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(Global::LoungeServer() != NULL);
}


//
// Send either sit-down or cancel sit-down pdu
// to the queue identified by 'index'.
//
void CLoungeDlg::joinOrRemoveQueue(int index, BOOL join)
{
  if (!Global::LoungeServer())
  { // leave if we don't have live connection to server
    return; 
  }

  int numColumns = 0;
  if (listCtrl_.GetHeaderCtrl())
    numColumns = listCtrl_.GetHeaderCtrl()->GetItemCount();

  if (index >= 0 && index < numColumns)
  {
    // Get the current game name
    CString s = getSelectedTabText();
    if (!s.IsEmpty())
    { // Find the queue to which the user wants to join
      Lounge::Queue& queue = Lounge::GetQueueIndex((LPCSTR)s, index);
      if (queue.lolimit_ != 0xffff)
      { 
        if (join)
        {
          queue.inQueue_ = TRUE;
          Global::LoungeServer()->sendLoungeSitDownPDU(queue.queueNumber_);
        }
        else
        {
          queue.inQueue_ = FALSE;
          Global::LoungeServer()->sendLoungeCancelSitDownPDU(queue.queueNumber_);
        }
      }
    }
  }
}


// Set the button to "Add" or "Remove" based
// on whether we're in the queue or not.
void CLoungeDlg::refreshButtons()
{
  CString s = getSelectedTabText();
  Game& game = Lounge::GetGame(string((LPCSTR)s));

  if (game.name_.size() > 0)
  {
    int i = 0;
    for (Queues::const_iterator it = game.queues_.begin(),
                                end = game.queues_.end();
         it != end;
         ++it, ++i)
    {
      const Queue& queue = *it;

      CWnd* pBtn = GetDlgItem(IDC_LD_BUTTONIDBASE + i);
      ASSERT(pBtn);

      if (queue.inQueue_)
      {
        if (pBtn)
          pBtn->SetWindowText(g_szRemoveFromQueue);
      }
      else
      {
        if (pBtn)
          pBtn->SetWindowText(g_szAddToQueue);
      }
    }
  }
}


void CLoungeDlg::queryTableDetails(USHORT table)
{
  if (Global::LoungeServer())
  {
    CRect r(0, 0, 0, 0);
    if (pTournamentDlg_ && pTournamentDlg_->IsWindowVisible())
    {
        pTournamentDlg_->GetWindowRect(&r);
        pTournamentDlg_->ShowWindow(SW_HIDE);
    }

    Global::LoungeServer()->sendVerboseTableQueryPDU(table);
    if (pDetailsDlg_)
    {
      if (!pDetailsDlg_->IsWindowVisible())
      {
          if (!r.IsRectEmpty())
              pDetailsDlg_->SetWindowPos(NULL,
                                         r.left,
                                         r.top,
                                         0, 0,
                                         SWP_NOSIZE|SWP_NOZORDER);
          pDetailsDlg_->ShowWindow(SW_NORMAL);
      }

      pDetailsDlg_->setQueryingTitle(table);
    }
  }
}

void CLoungeDlg::queryTournamentDetails(USHORT tournament)
{
  if (Global::LoungeServer())
  {
    CRect r(0, 0, 0, 0);
    if (pDetailsDlg_ && pDetailsDlg_->IsWindowVisible())
    {
        pDetailsDlg_->GetWindowRect(&r);
        pDetailsDlg_->ShowWindow(SW_HIDE);
    }

    Global::LoungeServer()->sendTournamentDetailsQueryPDU(tournament);
    if (pTournamentDlg_)
    {
      if (!pTournamentDlg_->IsWindowVisible())
      {
          if (!r.IsRectEmpty())
              pTournamentDlg_->SetWindowPos(NULL,
                                            r.left,
                                            r.top,
                                            0, 0,
                                            SWP_NOSIZE|SWP_NOZORDER);
          pTournamentDlg_->ShowWindow(SW_NORMAL);
      }

      pTournamentDlg_->setQueryingTitle(tournament);
    }
  }
}

void CLoungeDlg::refreshList(USHORT queueNumber)
{
  if (refresh_)
  {
    // Rebuild the whole list
    LRESULT lr = 0;
    OnSelchangeTab1(NULL, &lr);
  }
}

void CLoungeDlg::addPlayerToQueue(USHORT queueNum, const char* name)
{
  if (viewByWaitingLists())
  {
    // if queue is displayed, add name right away

    Game& game = Lounge::GetGameByQueue(queueNum);
    // Get the current game name
    CString s = getSelectedTabText();

    if (s == game.name_.c_str())
    { // the queue is currently displayed - update
      int col = FindColumn(game, queueNum);
      if (col != -1)
      {
        addItem(name, col);      
      }
    }
  }
}

void CLoungeDlg::removePlayerFromQueue(USHORT queueNum, const char* name)
{
  if (viewByWaitingLists())
  {
    // if queue is displayed, add name right away
    Game& game = Lounge::GetGameByQueue(queueNum);

    // Get the current game name
    CString s = getSelectedTabText();

    if (s == game.name_.c_str())
    { // the queue is currently displayed - update
      int col = FindColumn(game, queueNum);
      if (col != -1)
      {
        removeItem(name, col);
      }
    }
  }
}

void CLoungeDlg::addTableToQueue(USHORT queueNum, const char* name)
{
  if (viewByTables())
  {
    // if queue is displayed, add name right away

    Game& game = Lounge::GetGameByQueue(queueNum);
    // Get the current game name
    CString s = getSelectedTabText();

    if (s == game.name_.c_str())
    { // the queue is currently displayed - update
      int col = FindColumn(game, queueNum);
      if (col != -1)
      {
        addItem(name, col);
      }
    }
  }
}

void CLoungeDlg::removeTableFromQueue(USHORT queueNum, const char* name)
{
  if (viewByTables())
  {
    // if queue is displayed, add name right away
    Game& game = Lounge::GetGameByQueue(queueNum);

    // Get the current game name
    CString s = getSelectedTabText();

    if (s == game.name_.c_str())
    { // the queue is currently displayed - update
      int col = FindColumn(game, queueNum);
      if (col != -1)
      {
        removeItem(name, col);
      }
    }
  }
}

bool CLoungeDlg::viewByTables() const
{
  return (BST_CHECKED == GetDlgItem(IDC_LD_RADIO2)->SendMessage(BM_GETCHECK, 0, 0L));
}

bool CLoungeDlg::viewByWaitingLists() const
{
  return (BST_CHECKED == GetDlgItem(IDC_LD_RADIO1)->SendMessage(BM_GETCHECK, 0, 0L));
}

void CLoungeDlg::setRefreshFlag(BOOL b)
{
  refresh_ = b;
  if (refresh_)
    refreshList(0);
}


void CLoungeDlg::handlePDU(Network::PDUHeader* pPDU)
{
  using namespace Network;
  // All PDUs are handled here

  if (pPDU)
  {
    // Let the PDU implement its logic.
    pPDU->execute(NULL);

    // All PDU's are deallocated after
    // they have done their job.
    delete pPDU;
  }
}

void CLoungeDlg::OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult) 
{
  // Populate the list with current tab's (game's) data.
  resetList();
  int curSel = tabCtrl_.GetCurSel();
  populateList(curSel);

  // Force a repaint of the tabs - fixes repaint problems
  CRect r;
  tabCtrl_.GetWindowRect(&r);
  CRect rectItem;
  tabCtrl_.GetItemRect(0, &rectItem);
  r.bottom = r.top + tabCtrl_.GetRowCount() * rectItem.Height() + 3;
  ScreenToClient(&r);
  InvalidateRect(&r, TRUE);

  *pResult = 0;
}

void CLoungeDlg::OnViewByWaitList() 
{
  CDC* pDC = GetDC();
  if (pDC)
  {
    CPoint ofs(0, 2);
    radioBtnOn_.TransBlt(pDC, rectRadio1_.TopLeft() + ofs);
    radioBtnOff_.TransBlt(pDC, rectRadio2_.TopLeft() + ofs);
    ReleaseDC(pDC);
  }
  resetList();
  populateList(tabCtrl_.GetCurSel());
  GetDlgItem(IDC_LD_RADIO1)->SetFocus();
}

void CLoungeDlg::OnViewByTables() 
{
  CDC* pDC = GetDC();
  if (pDC)
  {
    CPoint ofs(0, 2);
    radioBtnOn_.TransBlt(pDC, rectRadio2_.TopLeft() + ofs);
    radioBtnOff_.TransBlt(pDC, rectRadio1_.TopLeft() + ofs);
    ReleaseDC(pDC);
  }
  resetList();
  populateList(tabCtrl_.GetCurSel());
  GetDlgItem(IDC_LD_RADIO2)->SetFocus();
}


void CLoungeDlg::OnClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
  CString s;
  
  LVHITTESTINFO lvhti;
  memset(&lvhti, 0, sizeof(lvhti));
  POINT pt;
  GetCursorPos(&pt);
  listCtrl_.ScreenToClient(&pt);
  lvhti.pt = pt;
  listCtrl_.SubItemHitTest(&lvhti);
  if (lvhti.flags & LVHT_ONITEMLABEL)
  {
    s = listCtrl_.GetItemText(lvhti.iItem, lvhti.iSubItem);
    TRACE1("Clicked on %s\n", s);
    listCtrl_.setSelection(lvhti.iItem, lvhti.iSubItem);

    if (s.Find("Tournament") != -1)
    {
      // Skip "Tournament"
      s = s.Right(s.GetLength() - 10);
      int tourney = -1;
      if (sscanf((LPCSTR)s, "%i", &tourney) == 1 && tourney >= 0)
        queryTournamentDetails(tourney);
    }
    else if (s.GetLength() > 5) // > "table"
    {
      // Skip "table"
      s = s.Right(s.GetLength() - 5);
      int table = -1;
      if (sscanf((LPCSTR)s, "%i", &table) == 1 && table >= 0)
        queryTableDetails(table);
    }
  }

  *pResult = 0;
}


void CLoungeDlg::OnRclickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
  *pResult = 0;
}


BOOL CLoungeDlg::PreTranslateMessage(MSG* pMsg) 
{
  BOOL rc = FALSE;

  // If focus is in some other than a button, eat ENTER and ESC
  // so the app won't exit accidentally
  if (pMsg->message == WM_KEYDOWN &&
      pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
  {
    // Eat ESC always
    if (pMsg->wParam == VK_ESCAPE)
      return TRUE;

    CWnd* pWnd = GetFocus();
    if (pWnd)
    {
      int ctrlID = pWnd->GetDlgCtrlID();
      if (!isButton(ctrlID))
        return TRUE; // Ignore
    }
  }

  rc = CDialog::PreTranslateMessage(pMsg);

  return rc;
}

BOOL CLoungeDlg::isButton(int ctrlID) const
{
  if (ctrlID == IDC_LOGIN ||
      ctrlID == IDC_CASHIER ||
      (ctrlID >= IDC_LD_BUTTONIDBASE &&
       ctrlID < IDC_LD_BUTTONIDBASE + buttons_.size()))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}



void CLoungeDlg::OnClose() 
{
  if (Global::LoungeServer())
  {
    Global::LoungeServer()->sendLoungeLogoutPDU();
    Global::LoungeServer()->sendLoungeLogoutCertainPDU();
  }

  if (pBanner_)
  {
    pBanner_->stop();
    delete pBanner_;
    pBanner_ = NULL;
  }
  CDialog::OnClose();
}


void CLoungeDlg::OnCashier() 
{
  CCashierDlg dlg(this);
  dlg.DoModal();
}

void CLoungeDlg::OnLogin() 
{
  if (Global::LoungeServer())
  { // we're connected - interpret as logout
    Global::LoungeServer()->sendLoungeLogoutPDU();
  }
  else
  { // we're not connected, interpret is login
    PostMessage(WM_COMMAND, MAKEWPARAM(ID_LOGIN, 0), 0);
  }	
}

HBRUSH CLoungeDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
    pDC->SetBkMode(TRANSPARENT);
	
	return hbr;
}

void CLoungeDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
    if (rectRadio1_.PtInRect(point))
    {
      GetDlgItem(IDC_LD_RADIO1)->SendMessage(BM_SETCHECK,
                                             MAKEWORD(BST_CHECKED, 0), 0L);
      GetDlgItem(IDC_LD_RADIO2)->SendMessage(BM_SETCHECK,
                                             MAKEWORD(BST_UNCHECKED, 0), 0L);
      OnViewByWaitList();
    }
    else if (rectRadio2_.PtInRect(point))
    {
      GetDlgItem(IDC_LD_RADIO2)->SendMessage(BM_SETCHECK,
                                             MAKEWORD(BST_CHECKED, 0), 0L);
      GetDlgItem(IDC_LD_RADIO1)->SendMessage(BM_SETCHECK,
                                             MAKEWORD(BST_UNCHECKED, 0), 0L);
      OnViewByTables();
    }
	
	CDialog::OnLButtonUp(nFlags, point);
}

#define LOGIN_TEST_ 1

void CLoungeDlg::OnButtonTournament() 
{
#ifndef LOGIN_TEST_
    if (!Global::LoungeServer())
    {   // not connected
        return;
    }
#endif

    EnableWindow(FALSE);

    // Close old connection, connect to new address
    //Global::Instance().closeConnections();

//    Lounge::EmptyAll();
    tabCtrl_.DeleteAllItems();
    resetList();

    bool rc = false;

    if (!InPlayMoney())
    {
        // Log in to the tournament server
        /*CString host = Registry::GetPlayMoneyHost("209.61.157.229");
        int port = Registry::GetPlayMoneyPort(PSPOT_LOUNGESERVER_PORT);

        rc = Global::Instance().loginToLoungeServer(
                                                GetSafeHwnd(),
                                                host, port,
                                                Network::Server::Username(),
                                                Network::Server::Password());
        */
        rc = TRUE;
        if (rc)
        {
            SetInPlayMoney(true);
            SetWindowText(g_szPlayMoneyTitle);
            GetDlgItem(IDC_BUTTON_TOURNAMENT)->SetWindowText("Real Money");
        }
    }
    else
    {
        /*
        CString host = Registry::GetServerHostName(PSPOT_LOUNGESERVER_HOST);
        int port = Registry::GetServerPort(PSPOT_LOUNGESERVER_PORT);
        rc = Global::Instance().loginToLoungeServer(
                                            GetSafeHwnd(),
                                            host,
                                            port,
                                            Network::Server::Username(),
                                            Network::Server::Password());
        */
        rc = TRUE;
        if (rc)
        {
            SetInPlayMoney(false);
            SetWindowText(g_szRealMoneyTitle);
            GetDlgItem(IDC_BUTTON_TOURNAMENT)->SetWindowText("Play Money");
        }
    }

    CFloorMap::Instance()->recalcLayout();
    EnableWindow(TRUE);
}

void CLoungeDlg::OnTimer(UINT nIDEvent) 
{
/*
#pragma message ("XXX: Login Testing")
    if (nIDEvent == 1)
    {
        OnButtonTournament();
    }
*/	
/*
    if (nIDEvent == ID_TIMER_BANNER)
    {
        doBanner();
    }
*/

	CDialog::OnTimer(nIDEvent);
}

void CLoungeDlg::OnButtonTestlogin() 
{
/*
    static bool b = true;
    if (b)
    {
        SetTimer(1, 10*1000, NULL);
        b = false;
    }
*/	
}

void CLoungeDlg::setBannerText(const CString& s, DWORD flags)
{
  if (!pBanner_)
  {
    CRect r(0, 0, 0, 0);
    CWnd* wnd = GetDlgItem(IDC_EDIT_BANNER);
    if (wnd)
    {
      wnd->GetWindowRect(&r);
      ScreenToClient(&r);
      pBanner_ = new CBanner(this, r);
    }
  }

  if (pBanner_)
  {
    pBanner_->setBannerText(s);
    pBanner_->start();
  }
/*
    bannerText_ = s;
    bannerPos_ = 0;
    if (bannerTimer_ != -1)
      KillTimer(bannerTimer_);

    bannerTimer_ = SetTimer(ID_TIMER_BANNER, 500, NULL);
    CWnd* wnd = GetDlgItem(IDC_EDIT_BANNER);
    if (wnd && wnd->GetSafeHwnd())
      wnd->ModifyStyle(ES_LEFT, ES_RIGHT);
*/
}

/*
void CLoungeDlg::doBanner()
{
    if (bannerText_.GetLength() > 0)
    {
        if (!bannerDir_)
        {
            bannerPos_++;
            if (bannerPos_ < bannerText_.GetLength())
            {
                CString s = bannerText_.Left(bannerPos_);
                GetDlgItem(IDC_EDIT_BANNER)->SetWindowText(s);
            }
            else
            {
                bannerDir_ = !bannerDir_;
                CWnd* wnd = GetDlgItem(IDC_EDIT_BANNER);
                if (wnd && wnd->GetSafeHwnd())
                  wnd->ModifyStyle(ES_RIGHT, ES_LEFT);
            }

        }
        else
        {
            bannerPos_--;
            if (bannerPos_ <= 0)
            {
              bannerPos_ = 0;
              bannerDir_ = !bannerDir_;
              CWnd* wnd = GetDlgItem(IDC_EDIT_BANNER);
              if (wnd && wnd->GetSafeHwnd())
                wnd->ModifyStyle(ES_LEFT, ES_RIGHT);
              return;
            }
    
            CString s = bannerText_.Right(bannerPos_);
            GetDlgItem(IDC_EDIT_BANNER)->SetWindowText(s);
        }
    }
}
*/
void CLoungeDlg::OnRadioRealmoney() 
{
  SetInPlayMoney(FALSE);
  tabCtrl_.DeleteAllItems();
  resetList();
  CFloorMap::Instance()->recalcLayout();
}

void CLoungeDlg::OnRadioPlaymoney2() 
{
  SetInPlayMoney(TRUE);
  tabCtrl_.DeleteAllItems();
  resetList();
  CFloorMap::Instance()->recalcLayout();
}
