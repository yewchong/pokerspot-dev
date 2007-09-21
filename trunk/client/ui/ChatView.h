#ifndef __psi_chatview_h__
#define __psi_chatview_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CChatView
  Created  : 25.10.1999

  OVERVIEW : CChatView implements the chat pane view.

 ****************************************************************************/

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <string>
#include <list>


/////////////////////////////////////////////////////////////////////////////
// CChatView view

class CChatView : public CScrollView
{
public:
  static void SetUserColor(COLORREF rgb);
  static void SetNormalColor(COLORREF rgb);
  static void SetCriticalColor(COLORREF rgb);

  enum {
    MaxChatMessages = 50
  };

  struct ChatEntry
  {
    std::string text_;
    DWORD       flags_;
    int         height_;

    ChatEntry() : flags_(0), height_(0) {}

    ChatEntry(const std::string& t, int height, DWORD f = 0)
      : text_(t), flags_(f), height_(height) {}

    ChatEntry& operator = (const ChatEntry& ce)
    {
      if (this != &ce)
      {
        text_ = ce.text_;
        flags_ = ce.flags_;
        height_ = ce.height_;
      }
      return *this;
    }
  };

  // The chat view maintains a lists of chat entries
  typedef std::list<ChatEntry> ChatList;

  static CChatView* Instance() { return pInst_; }

  CChatView();

protected:
  DECLARE_DYNCREATE(CChatView)

// Attributes
public:

// Operations
public:
  enum ChatFlags {
    CF_System       = 1,
    CF_Fatal        = 2,
    CF_GameNormal   = 4,
    CF_GameCritical = 8,
  };
  void addUserMessage(LPCTSTR msg, DWORD flags, BOOL invalidate = TRUE);
  void addDealerMessage(LPCTSTR msg, DWORD flags, BOOL invalidate = TRUE);
  void addDebugMessage(LPCTSTR msg, DWORD flags, BOOL invalidate = TRUE);
  void setCurrentView(int);

private:
  int  renderChatEntry(CDC* pDC, ChatEntry* chatEntry, int line, const CRect& r);
  void addChatMessage(ChatList*, LPCTSTR msg, DWORD flags, BOOL invalidate);
  

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CChatView)
public:
  virtual void OnInitialUpdate();
protected:
  virtual void OnDraw(CDC* pDC); // overridden to draw this view
  //}}AFX_VIRTUAL

// Implementation
protected:
  virtual ~CChatView();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
  //{{AFX_MSG(CChatView)
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
  afx_msg void OnEditCopy();
  afx_msg void OnClear();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

private:
  static CChatView* pInst_;

  ChatList allChats_;
  ChatList dealerChats_;
  ChatList userChats_;
  ChatList* pCurrentChat_;

  // Font used for system messages
  CFont fontSys_;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif 
