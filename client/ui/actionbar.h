#ifndef __psi_actionbar_h__
#define __psi_actionbar_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CActionBar
  Created  : 25.10.1999

  OVERVIEW : CActionBar implements the floating action button bar.

 ****************************************************************************/


#include "ui/resource.h"
#include "ui/actionbutton.h"
#include "ui/checkbutton.h"


class CChips;

/////////////////////////////////////////////////////////////////////////////
// CActionBar dialog

class CActionBar : public CDialog
{
// Construction
public:
  CActionBar();
  ~CActionBar();
  BOOL Create(CWnd* pParent);

  static CActionBar* pInst_;
  static CActionBar* Instance() { return pInst_; }
  static void ShowToolTips(BOOL);
  static void UncheckActionButtons();
  static void UncheckCallButton();
  static void UncheckRaiseButton();
  static void UncheckFoldButton();
         void uncheckActionButtons();
         void uncheckCallButton();
         void uncheckRaiseButton();
         void uncheckFoldButton();
         void updateButtons();
  static void CheckSitOut(BOOL);
         void checkSitOut(BOOL);

  static void ResetAnteCheckbox();

  void dock(BOOL);
  BOOL isDocked() const;
  void attach();      
  int  getMinWidth() const;

public:
  enum {
    NumButtons = 3
  };

  enum BtnFlag
  {
    Btn_First  = 1,
    Btn_Second = 2,
    Btn_Third  = 4,
    Btn_Fourth = 8,
    Btn_Fifth  = 16,
    Btn_All    = 0xffff
  };
  void enableButtons(DWORD dwFlags);
  void setSpread(const CChips& lo, const CChips& hi);
  CChips getRaise() const;
  void enableCheckBoxes(BOOL);
  void animate(DWORD dt);
  void stopAnimate();
  void setButtonText(int, const CString&);
  CButton* getButton(int i) { return (i >= 0 && i < NumButtons) ? &btn_[i] : 0; }
  void onCheckPostBlind() { OnCheckPostblind(); }
  void onCheckSitout() { OnCheckSitout(); }
  void onCheckCallInTurn() { OnCheckCall(); }
  void onCheckRaiseInTurn() { OnCheckRaise(); }
  void onCheckFoldInTurn() { OnCheckFold(); }
  void onCheckMuckHand() { OnCheckMuckHand(); }
  void resetCheckStates();
  void saveWidth();
  void setTournament();

private:
  CActionButton btn_[NumButtons];
  CCheckButton  checkBtn_[6];
  CBrush        brBkgnd_;
  CSliderCtrl   slider_;

public:

// Dialog Data
  //{{AFX_DATA(CActionBar)
	enum { IDD = IDD_ACTIONBAR };
	//}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CActionBar)
protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  virtual BOOL OnInitDialog();
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CActionBar)
  afx_msg void OnClose();
  afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
  afx_msg void OnCheckCall();
  afx_msg void OnCheckRaise();
  afx_msg void OnCheckFold();
  afx_msg void OnCheckPostblind();
  afx_msg void OnCheckSitout();
  afx_msg void OnCheckMuckHand();
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeRaiseamount();
	//}}AFX_MSG
  afx_msg BOOL OnToolTipNotify(UINT id, NMHDR *pNMHDR,
                               LRESULT *pResult);

  DECLARE_MESSAGE_MAP()

private:
  BOOL isChecked(UINT idc);
  DWORD ticks_;
  int minWidth_;
};

#define DefaultActionBarDock 1

#endif
