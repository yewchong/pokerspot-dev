// LoungeDlg.h : header file
//

#ifndef __psi_loungedlg_h__
#define __psi_loungedlg_h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include "listctrl.h"
#include "tabctrl.h"
#include "base/transparentbitmap.h"
#include "ui/chatdlg.h"

namespace Base { class JPGImage; }
namespace Network { struct PDUHeader; }

class CTableDetailsDlg;
class CTournamentDetailsDlg;
class CBanner;


class CLoungeDlg : public CDialog
{
public:
  DECLARE_DYNAMIC(CLoungeDlg);

public:
  CLoungeDlg(CWnd* pParent = NULL);
  ~CLoungeDlg();

  static BOOL InPlayMoney();

public:
  void setStats(int players, int tables);
  void disconnect();
  void addGamesTabs();
  void populateList(LPCTSTR gamename);
  void populateList(int tabindex);
  void resetList();
  CString getSelectedTabText();

  void refreshList(USHORT queueNum);
  void addPlayerToQueue(USHORT queueNum, const char* name);
  void removePlayerFromQueue(USHORT queueNum, const char* name);
  void addTableToQueue(USHORT queueNum, const char* name);
  void removeTableFromQueue(USHORT queueNum, const char* name);
  void setRefreshFlag(BOOL);

  bool viewByTables() const;
  bool viewByWaitingLists() const;

  void restoreSettings();
  void saveSettings();

  void joinOrRemoveQueue(int index, BOOL join);
  void refreshButtons();
  void queryTableDetails(USHORT table);
  void queryTournamentDetails(USHORT tournament);

  void setBannerText(const CString& s, DWORD flags);

  CTableDetailsDlg* getDetailsDlg() const { return pDetailsDlg_; }
  CTournamentDetailsDlg* getTournamentDlg() const { return pTournamentDlg_; }
  CFont* getFont() { return &font_; }

  //{{AFX_DATA(CLoungeDlg)
  enum { IDD = IDD_LOUNGE_DIALOG };
  // NOTE: the ClassWizard will add data members here
  TabCtrl  tabCtrl_;
  ListCtrl listCtrl_;
  //}}AFX_DATA

  //{{AFX_VIRTUAL(CLoungeDlg)
public:
  virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
  virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
  virtual BOOL DestroyWindow();
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  virtual BOOL OnInitDialog();
  //}}AFX_VIRTUAL

protected:
  HICON m_hIcon;

  //{{AFX_MSG(CLoungeDlg)
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnViewByWaitList();
  afx_msg void OnViewByTables();
  afx_msg void OnUpdateLogin(CCmdUI*);
  afx_msg void OnUpdateLogout(CCmdUI*);
  afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnRclickList(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnClose();
  afx_msg void OnCashier();
  afx_msg void OnLogin();
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnButtonTournament();
  afx_msg void OnTimer(UINT nIDEvent);
  afx_msg void OnButtonTestlogin();
	afx_msg void OnRadioRealmoney();
	afx_msg void OnRadioPlaymoney2();
	//}}AFX_MSG
  DECLARE_MESSAGE_MAP()

private:
  CBanner* pBanner_;
  CChatDlg* pChatDlg_;
  CTableDetailsDlg* pDetailsDlg_;
  CTournamentDetailsDlg* pTournamentDlg_;

  void addTab(LPCTSTR tabtitle);
  void addColumn(LPCTSTR coltitle, int totalcolumns, BOOL inQueue);
  void addItem(LPCTSTR text, int column);
  void removeItem(LPCTSTR text, int column);
  void adaptTabCtrl();

  void doLogin();
  void doLogout();
  void handlePDU(Network::PDUHeader*);
  BOOL isButton(int) const;

  void addInitialData();
  std::list<CButton*> buttons_;  

  struct HiliteItem
  {
    enum { Num = 2 };
    HiliteItem() 
      : count_(Num), item_(0), subItem_(0), rect_(0, 0, 0, 0) {}
    HiliteItem(int item, int subItem, const CRect& r)
      : count_(Num), item_(item), subItem_(subItem_), rect_(r) {}
    HiliteItem(const HiliteItem& hitem)
      : count_(Num), item_(hitem.item_), subItem_(hitem.subItem_), rect_(hitem.rect_) {}
    int item_, subItem_;
    int count_;
    CRect rect_;    
  };
  std::list<HiliteItem> hilites_;
  UINT timer_;

  CFont font_;
  Base::JPGImage* pImage_;

  CRect rectRadio1_, rectRadio2_;
  int radioYOffset_;
  // Fake owner-draw radio button:
  Base::TransparentBitmap radioBtnOn_,
                          radioBtnOff_;

  BOOL refresh_;
};

//{{AFX_INSERT_LOCATION}}

#endif 
