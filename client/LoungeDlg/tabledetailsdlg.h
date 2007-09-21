#ifndef __psi_tabledetailsdlg_h__
#define __psi_tabledetailsdlg_h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// tabledetailsdlg.h : header file
//

class CChips;

/////////////////////////////////////////////////////////////////////////////
// CTableDetailsDlg dialog

class CTableDetailsDlg : public CDialog
{
// Construction
public:
  CTableDetailsDlg(CWnd* pParent = NULL);

  void setQueryingTitle(USHORT table);
  void setFinishedTitle();

  void setAvgPot     (const CChips& avgpot);
  void setFlopPrct   (USHORT flopPrct);
  void setQueueSize  (USHORT queueSize);
  void setNumPlayers (USHORT numPlayers);
  
  void resetListContents() { listCtrl_.DeleteAllItems(); }
  void addListItem(const char* player, const char* city, const char* chips);

// Dialog Data
  //{{AFX_DATA(CTableDetailsDlg)
  enum { IDD = IDD_TABLEDETAILS };
  // NOTE: the ClassWizard will add data members here
  CListCtrl listCtrl_;
  //}}AFX_DATA

  //{{AFX_VIRTUAL(CTableDetailsDlg)
protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  //}}AFX_VIRTUAL

// Implementation
protected:

  //{{AFX_MSG(CTableDetailsDlg)
  virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnObserve();
	//}}AFX_MSG
  DECLARE_MESSAGE_MAP()

private:
  USHORT table_;
};

//{{AFX_INSERT_LOCATION}}

#endif 
