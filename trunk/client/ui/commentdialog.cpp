// commentdialog.cpp : implementation file
//

#include "stdafx.h"
#include "shell/poker.h"
#include "ui/commentdialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCommentDialog dialog


CCommentDialog::CCommentDialog(const CString& username, 
                               CWnd* pParent /*=NULL*/)
  :
  username_(username),
  CDialog(CCommentDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCommentDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CCommentDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCommentDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCommentDialog, CDialog)
	//{{AFX_MSG_MAP(CCommentDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCommentDialog message handlers

extern CString GetComment(const CString& username);

BOOL CCommentDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

    CString title;
    title.Format("Notes on player %s", username_);

    CString comment = GetComment(username_);
	
    SetWindowText(title);
    GetDlgItem(IDC_EDIT1)->SetWindowText(comment);
    GetDlgItem(IDC_EDIT1)->SetFocus();
	
	return FALSE; // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CCommentDialog::OnOK() 
{
  bool ok = false;

  CString text;
  GetDlgItem(IDC_EDIT1)->GetWindowText(text);
  CString fileName;
  fileName.Format("comment/%s.txt", username_);

  TRY
  {
    if (text.GetLength() > 0)
    {
      CFile f;
      CFileException ex;
      if (!f.Open(fileName, 
                  CFile::modeCreate|CFile::modeWrite,
                  &ex))
      {
        throw new CFileException(ex.m_cause, ex.m_lOsError, ex.m_strFileName);
      }
      f.Write((LPCSTR)text, text.GetLength());
      ok = true;
    }
    else
    { // Setting the contents empty means players wants
      // to remove the comment file
      CFile::Remove(fileName);
      ok = true;
    }
  }
  CATCH_ALL(e)
  {
    ok = false;
    e->ReportError();
  }
  END_CATCH_ALL

  if (ok)
  {
    CDialog::OnOK();
  }
}