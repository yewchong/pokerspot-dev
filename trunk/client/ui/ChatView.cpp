/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CChatView
  Created  : 25.10.1999

  OVERVIEW : CChatView implements the chat pane view.

 ****************************************************************************/

#include "stdafx.h"
#include "chatview.h"
#include "global.h"
#include "ui/cmdid.h"
#include "base/misc.h"
#include "base/sync.h"
#include "network/network.h"
#include <algorithm>
using namespace std;

//
// The meaning of the chat tabs is now:
// 
// allChats_: contains all messages
// dealerChats_:  contains dealer messages
// userChats_: contains user messages

const int Offset = 2;

// This functor computes the sum of
// the heights of chat entries in a list
struct SumHeightsFtor :
    public unary_function<const CChatView::ChatEntry&, bool>
                                    
{
  SumHeightsFtor(int* pRes)
    : pRes_(pRes)
  {}

  bool operator()(const CChatView::ChatEntry& ce)
  {
      *pRes_ += ce.height_ + Offset;
      return true;
  }

  int* pRes_;
};

// This functor computes the sizes of the
// texts in chat entries a list and stores
// the size to the chat entry.
struct ComputeHeightsFtor :
    public unary_function<CChatView::ChatEntry&, bool>
                                    
{
  ComputeHeightsFtor(CDC* pDC, CRect* pRect)
    : pDC_(pDC), pRect_(pRect)
  {}

  bool operator()(CChatView::ChatEntry& ce)
  {
    CRect rectOut(5, 0, pRect_->right, 0);
    ce.height_ = pDC_->DrawText(ce.text_.c_str(),
                                ce.text_.size(),
                                &rectOut,
                                DT_WORDBREAK|DT_CALCRECT|DT_NOPREFIX);      
    return true;
  }

  CDC* pDC_;
  CRect* pRect_;
};

  

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
  LPCTSTR g_szClearChat = _T("C&lear");
  LPCTSTR g_szCopyToClipboard = _T("&Copy to Clipboard");

  COLORREF g_rgbUser     = RGB(0, 0, 0);
  COLORREF g_rgbNormal   = RGB(0, 128, 0);
  COLORREF g_rgbCritical = RGB(255, 125, 0);
}

/////////////////////////////////////////////////////////////////////////////
// CChatView

void CChatView::SetUserColor(COLORREF rgb)
{
  g_rgbUser = rgb;
  if (CChatView::Instance())
    CChatView::Instance()->Invalidate();
}


void CChatView::SetNormalColor(COLORREF rgb)
{
  g_rgbNormal = rgb;
  if (CChatView::Instance())
    CChatView::Instance()->Invalidate();
}


void CChatView::SetCriticalColor(COLORREF rgb)
{
  g_rgbCritical = rgb;
  if (CChatView::Instance())
    CChatView::Instance()->Invalidate();
}


CChatView* CChatView::pInst_ = 0;

IMPLEMENT_DYNCREATE(CChatView, CScrollView)

CChatView::CChatView()
  :
  pCurrentChat_(&userChats_)
{
  pInst_ = this;

  CFont* pFont = Global::GetFont();
  if (pFont)
  {
    LOGFONT lf;
    pFont->GetLogFont(&lf);
    lf.lfWeight = FW_BOLD;
    fontSys_.CreateFontIndirect(&lf);
  }

#ifdef PSPOT_TABLE_MODULE_
  dealerChats_.push_back(ChatEntry("Dealer messages",
                                   Global::GetCharHeight()));
  userChats_.push_back(ChatEntry("User messages",
                                 Global::GetCharHeight()));
#endif
}


CChatView::~CChatView()
{
  pInst_ = 0;
}


BEGIN_MESSAGE_MAP(CChatView, CScrollView)
  //{{AFX_MSG_MAP(CChatView)
  ON_WM_SIZE()
  ON_WM_CONTEXTMENU()
  ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
  ON_COMMAND(ID_CLEARCHAT, OnClear)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChatView drawing

struct RAIISelectFont
{
  RAIISelectFont(CDC* pDC, CFont* pFont)
    : pDC_(pDC)
  {
    pOldFont_ = pDC->SelectObject(pFont);
  }
  ~RAIISelectFont()
  {
    pDC_->SelectObject(pOldFont_);
  }
  CDC*   pDC_;
  CFont* pOldFont_;
};


void CChatView::OnDraw(CDC* pDC)
{
  ASSERT(pCurrentChat_);

  int charHeight = Global::GetCharHeight();
  CRect rectClient;
  GetClientRect(&rectClient);

  CFont* pDefaultFont = 
    CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
  CFont* pOldFont =
    (CFont*)pDC->SelectObject(pDefaultFont);
  pDC->SetBkMode(TRANSPARENT);

  int line = 0;

  COLORREF oldColor = 0;

  int ypos = Offset;
  // Render chat text 
  for (ChatList::reverse_iterator i = pCurrentChat_->rbegin(),
                                  e = pCurrentChat_->rend();
       i != e;
       ++i, ++line)
  {
    if (i->flags_ & CF_Fatal)
      oldColor = pDC->SetTextColor(RGB(255, 0, 0));
    else if (i->flags_ & CF_GameNormal)
      oldColor = pDC->SetTextColor(g_rgbNormal);
    else if (i->flags_ & CF_GameCritical)
      oldColor = pDC->SetTextColor(g_rgbCritical);
    else
      oldColor = pDC->SetTextColor(g_rgbUser);

    int height = renderChatEntry(pDC, &(*i),
                                 ypos,
                                 rectClient);
    ypos += height + Offset;

    pDC->SetTextColor(oldColor);
  }

  pDC->SelectObject(pOldFont);
}

int CChatView::renderChatEntry(CDC* pDC,
                               ChatEntry* chatEntry,
                               int ystart,
                               const CRect& rectClient)
{
    int rc = 1;

    CString s(chatEntry->text_.c_str());

    CRect rectOut(5, ystart, rectClient.right, rectClient.bottom);
    return pDC->DrawText(s, rectOut, DT_WORDBREAK|DT_NOCLIP|DT_NOPREFIX);
}

void CChatView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
  CMenu m;
  if (m.CreatePopupMenu())
  {
    m.AppendMenu(MF_STRING, ID_EDIT_COPY, g_szCopyToClipboard);
    m.AppendMenu(MF_STRING, ID_CLEARCHAT, g_szClearChat);
    m.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,
                     point.x, point.y, this);
  }	
}


void CChatView::OnEditCopy()
{
  std::string s, newline("\r\n");

  // Copy chat text to buffer
  for (ChatList::reverse_iterator i = pCurrentChat_->rbegin(),
                                  e = pCurrentChat_->rend();
       i != e;
       ++i)
  {
    ChatEntry e = (*i);

    if (s.size() > 0)
      s += newline;
    s += e.text_;
  }

  if (OpenClipboard() && ::EmptyClipboard())
  {
    HGLOBAL hMem = GlobalAlloc(GMEM_DDESHARE, (s.size() + 1)); 
    if (hMem == NULL)
    {
      AfxMessageBox("Error: out of memory", MB_ICONEXCLAMATION);
    }
    else
    {
      // Lock the handle and copy the text to the buffer.  
      LPSTR pmem = (LPSTR)GlobalLock(hMem); 
      memcpy(pmem, s.c_str(), s.size()); 
      pmem[s.size()] = '\0';
      GlobalUnlock(hMem);

      // Place the handle on the clipboard.  
      ::SetClipboardData(CF_TEXT, hMem); 
    }

    CloseClipboard();
  }
  else
  {
    AfxMessageBox("Error: can't open clipboard", MB_ICONEXCLAMATION);
  }

}


void CChatView::OnClear()
{
  if (pCurrentChat_)
  {
    pCurrentChat_->clear();
    Invalidate();
  }
}

/////////////////////////////////////////////////////////////////////////////
// CChatView diagnostics

#ifdef _DEBUG
void CChatView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CChatView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChatView message handlers


void CChatView::OnInitialUpdate() 
{
  CScrollView::OnInitialUpdate();

  CSize sizeLine(0, Global::GetCharHeight());
  CSize sizeTotal(0, 10);
  SetScrollSizes(MM_TEXT, sizeTotal, sizeDefault, sizeLine);
}


void CChatView::OnSize(UINT nType, int cx, int cy) 
{
  CScrollView::OnSize(nType, cx, cy); 

  // Go through all chat texts are recompute sizes!
  CRect rectClient;
  GetClientRect(&rectClient);
  CDC* pDC = GetDC();
  if (pDC)
  {
    CFont* pFont = 
      CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
    CFont* pOldFont =
      (CFont*)pDC->SelectObject(pFont);

    ComputeHeightsFtor ftor(pDC, &rectClient);
    for_each(allChats_.begin(), allChats_.end(), ftor);
    for_each(dealerChats_.begin(), dealerChats_.end(), ftor);
    for_each(userChats_.begin(), userChats_.end(), ftor);

    pDC->SelectObject(pOldFont);
    ReleaseDC(pDC);
    pDC = NULL;
  }

}

/////////////////////////////////////////////////////////////////////////////
// CChatView application mfunctions


void CChatView::addUserMessage(LPCTSTR msg, DWORD flags, BOOL invalidate)
{
  addChatMessage(&allChats_, msg, flags, invalidate);
  addChatMessage(&userChats_, msg, flags, invalidate);
}


void CChatView::addDealerMessage(LPCTSTR msg, DWORD flags, BOOL invalidate)
{
  addChatMessage(&allChats_, msg, flags, invalidate);
  addChatMessage(&dealerChats_, msg, flags, invalidate);
}

void CChatView::addDebugMessage(LPCTSTR msg, DWORD flags, BOOL invalidate)
{
  // FOR NOW:
  addChatMessage(&userChats_, msg, flags, invalidate);
}


void CChatView::addChatMessage(ChatList* pChatList,
                               LPCTSTR msg,
                               DWORD flags,
                               BOOL invalidate)
{
  int height = Global::GetCharHeight();

  CDC* pDC = GetDC();
  if (pDC)
  {
    CString s(msg);

    CFont* pFont = 
      CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
    CFont* pOldFont =
      (CFont*)pDC->SelectObject(pFont);

    CRect rectClient;
    GetClientRect(&rectClient);
    CRect rectOut(5, 0, rectClient.right, 0);
    height = pDC->DrawText(msg, rectOut, DT_WORDBREAK|DT_CALCRECT|DT_NOPREFIX);

    pDC->SelectObject(pOldFont);
    ReleaseDC(pDC);
    pDC = NULL;
  }


  pChatList->push_front(ChatEntry(msg, height, flags));
  if (pChatList->size() > MaxChatMessages)
    pChatList->pop_back();

  if (pCurrentChat_ == pChatList)
  { // If added to active list, refresh
    CRect r;
    GetClientRect(&r);

    int charHeight = Global::GetCharHeight();
    if (charHeight == 0) charHeight = 1;

    int totalHeight = 0;
    SumHeightsFtor ftor(&totalHeight);
    for_each(pChatList->begin(), pChatList->end(), ftor);

    CSize sizeLine(1, charHeight);
    CSize sizePage(1, r.Height());
    totalHeight += charHeight;
    //CSize sizeTotal(1, (pChatList->size() + 1) * charHeight);
    CSize sizeTotal(1, totalHeight);
    SetScrollSizes(MM_TEXT, sizeTotal, sizePage, sizeLine);

    // scroll to last line
    if (sizeTotal.cy > r.Height())
      ScrollToPosition(CPoint(0, sizeTotal.cy));

    if (invalidate)
      Invalidate();
  }
}


void CChatView::setCurrentView(int i)
{
  if (i == 0)
    pCurrentChat_ = &allChats_;
  else if (i == 1)
    pCurrentChat_ = &dealerChats_;
  else if (i == 2)
    pCurrentChat_ = &userChats_;
  else
  {
    ASSERT(FALSE);
  }

  int charHeight = Global::GetCharHeight();
  if (charHeight == 0) charHeight = 1;

  CRect r;
  GetClientRect(&r);

  int totalHeight = 0;
  SumHeightsFtor ftor(&totalHeight);
  for_each(pCurrentChat_->begin(), pCurrentChat_->end(), ftor);
  totalHeight += charHeight;

  CSize sizeLine(1, charHeight);
  CSize sizePage(1, r.Height());
  CSize sizeTotal(1, totalHeight);
  SetScrollSizes(MM_TEXT, sizeTotal, sizePage, sizeLine);

  Invalidate();
}

struct PostDebugPrint : public Network::PDUHeader
{
  void execute(Base::GameState*)
  {
    if (CChatView::Instance())
      CChatView::Instance()->addDebugMessage(msg_, flags_, TRUE);
  }

  PostDebugPrint(const char* msg, DWORD flags)
    :
    PDUHeader(u_int16_t(0), sizeOf(), "x", "x"),
    msg_  (msg),
    flags_(flags)
  {}

  CString msg_;
  DWORD   flags_;
};


// PDUDebugPrint will be called from the PDU
// reader thread. We therefore put it through
// the message pump to make sure its thread-safe.
void PDUDebugPrint(const char* msg, DWORD flags)
{ 
  CView* pView = Base::GetActiveView();
  CChatView* pChat = CChatView::Instance();

  if (pChat && ::IsWindow(pChat->GetSafeHwnd()))
  {
    if (pView && ::IsWindow(pView->GetSafeHwnd()))
    { // Post to active view
      ::PostMessage(pView->GetSafeHwnd(),
                  WM_COMMAND,
                  MAKEWPARAM(ID_PDURECEIVED, 0),
                  reinterpret_cast<LPARAM>(new PostDebugPrint(
                                            msg, flags)));
    }
    else
    {
      CWnd* pWnd = AfxGetMainWnd();
      if (pWnd)
      { // Post to main window
        ::PostMessage(pWnd->GetSafeHwnd(),
                      WM_COMMAND,
                      MAKEWPARAM(ID_PDURECEIVED, 0),
                      reinterpret_cast<LPARAM>(new PostDebugPrint(
                                               msg, flags)));  
      }
    }
  }
}

void SysDebugPrint(const char* msg, DWORD flags)
{ 
  CView* pView = Base::GetActiveView();
  CChatView* pChat = CChatView::Instance();

  if (pChat && ::IsWindow(pChat->GetSafeHwnd()))
  {
    pChat->addDebugMessage(msg, flags);
  }
}

