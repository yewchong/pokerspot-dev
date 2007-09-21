// creditdarddlg.cpp : implementation file
//

#include "stdafx.h"
#include "loungedlg/lounge.h"
#include "loungedlg/creditcarddlg.h"
#include "network/server.h"
#include "network/pducreditcardedit.h"
#include "ui/global.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCreditCardDlg dialog

CCreditCardDlg* CCreditCardDlg::pDlg_ = 0;
std::vector<std::string> CCreditCardDlg::cctypes_;


CCreditCardDlg::CCreditCardDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreditCardDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCreditCardDlg)
	cc1_number_ = _T("");
	cc2_number_ = _T("");
	cc3_number_ = _T("");
	cc4_number_ = _T("");
	//}}AFX_DATA_INIT

  pDlg_ = this;
}


CCreditCardDlg::~CCreditCardDlg()
{
  pDlg_ = 0;
}


void CCreditCardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreditCardDlg)
	DDX_Text(pDX, IDC_CC1_NUMBER, _cc1_number_);
	DDV_MaxChars(pDX, _cc1_number_, 16);
	DDX_Text(pDX, IDC_CC2_NUMBER, _cc2_number_);
	DDV_MaxChars(pDX, _cc2_number_, 16);
	DDX_Text(pDX, IDC_CC3_NUMBER, _cc3_number_);
	DDV_MaxChars(pDX, _cc3_number_, 16);
	DDX_Text(pDX, IDC_CC4_NUMBER, _cc4_number_);
	DDV_MaxChars(pDX, _cc4_number_, 16);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCreditCardDlg, CDialog)
	//{{AFX_MSG_MAP(CCreditCardDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreditCardDlg message handlers

BOOL CCreditCardDlg::OnInitDialog() 
{
  SetWindowText("Credit Cards - querying...");

  if (Global::LoungeServer())
  {
    static bool typesToo = true;
    Global::LoungeServer()->sendCCardQueryPDU2(typesToo);
    typesToo = false;
  }

  CDialog::OnInitDialog();

  if (cctypes_.size() > 0)
  {
    int numTypes = cctypes_.size();

    ((CComboBox*)GetDlgItem(IDC_CC1_TYPE))->InitStorage(numTypes, 10);
    ((CComboBox*)GetDlgItem(IDC_CC2_TYPE))->InitStorage(numTypes, 10);
    ((CComboBox*)GetDlgItem(IDC_CC3_TYPE))->InitStorage(numTypes, 10);
    ((CComboBox*)GetDlgItem(IDC_CC4_TYPE))->InitStorage(numTypes, 10);

    for (int i = 0; i < numTypes; i++)
    {
      CString cctype = cctypes_[i].c_str();

      ((CComboBox*)GetDlgItem(IDC_CC1_TYPE))->AddString(cctype);
	  ((CComboBox*)GetDlgItem(IDC_CC2_TYPE))->AddString(cctype);
      ((CComboBox*)GetDlgItem(IDC_CC3_TYPE))->AddString(cctype);
      ((CComboBox*)GetDlgItem(IDC_CC4_TYPE))->AddString(cctype);  
    }

    ((CComboBox*)GetDlgItem(IDC_CC1_TYPE))->SetCurSel(0);
    ((CComboBox*)GetDlgItem(IDC_CC2_TYPE))->SetCurSel(0);
    ((CComboBox*)GetDlgItem(IDC_CC3_TYPE))->SetCurSel(0);
    ((CComboBox*)GetDlgItem(IDC_CC4_TYPE))->SetCurSel(0);
  }


  CString str;
  
  for (int i = 1; i < 13; i++)
  {
    if (i < 10)
      str.Format("0%i", i);
    else
      str.Format("%i", i);
    ((CComboBox*)GetDlgItem(IDC_CC1_EXPRMONTH))->AddString(str);
    ((CComboBox*)GetDlgItem(IDC_CC2_EXPRMONTH))->AddString(str);
    ((CComboBox*)GetDlgItem(IDC_CC3_EXPRMONTH))->AddString(str);
    ((CComboBox*)GetDlgItem(IDC_CC4_EXPRMONTH))->AddString(str);
  }

  ((CComboBox*)GetDlgItem(IDC_CC1_EXPRMONTH))->SetCurSel(0);
  ((CComboBox*)GetDlgItem(IDC_CC2_EXPRMONTH))->SetCurSel(0);
  ((CComboBox*)GetDlgItem(IDC_CC3_EXPRMONTH))->SetCurSel(0);
  ((CComboBox*)GetDlgItem(IDC_CC4_EXPRMONTH))->SetCurSel(0);
	
  for (i = 1; i < 10; i++)
  {
    str.Format("200%i", i);
    ((CComboBox*)GetDlgItem(IDC_CC1_EXPRYEAR))->AddString(str);
    ((CComboBox*)GetDlgItem(IDC_CC2_EXPRYEAR))->AddString(str);
    ((CComboBox*)GetDlgItem(IDC_CC3_EXPRYEAR))->AddString(str);
    ((CComboBox*)GetDlgItem(IDC_CC4_EXPRYEAR))->AddString(str);
  }

  ((CComboBox*)GetDlgItem(IDC_CC1_EXPRYEAR))->SetCurSel(0);
  ((CComboBox*)GetDlgItem(IDC_CC2_EXPRYEAR))->SetCurSel(0);
  ((CComboBox*)GetDlgItem(IDC_CC3_EXPRYEAR))->SetCurSel(0);
  ((CComboBox*)GetDlgItem(IDC_CC4_EXPRYEAR))->SetCurSel(0);

  enableControls(false);

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}


// Populate controls and save initial values so we know
// if user has changed them.
void CCreditCardDlg::AddCreditCards(int num_cards,
                                    PDUCreditCardReply2::CardInfo* pCI)
{
  if (!pDlg_) return;

  CWnd* pEditBox = 0;
 
  for (int i = 0; i < num_cards; i++, pCI++)
  {
    if (pCI->digits_[0] == 0)
      continue;

    CString s;
    s.Format("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
                              pCI->digits_[0],
                              pCI->digits_[1],
                              pCI->digits_[2],
                              pCI->digits_[3],
                              pCI->digits_[4],
                              pCI->digits_[5],
                              pCI->digits_[6],
                              pCI->digits_[7],
                              pCI->digits_[8],
                              pCI->digits_[9],
                              pCI->digits_[10],
                              pCI->digits_[11],
                              pCI->digits_[12],
                              pCI->digits_[13],
                              pCI->digits_[14],
                              pCI->digits_[15]);


    pEditBox = 0;
    
    int typeIndex = pCI->card_type_;
    CString cctype;
    if (typeIndex >= 0 && typeIndex < cctypes_.size())
      cctype = cctypes_[typeIndex].c_str();

    CString expyr, expmo;
    expyr.Format("20%c%c", pCI->expyr_[0], pCI->expyr_[1]);
    expmo.Format("%c%c", pCI->expmo_[0], pCI->expmo_[1]);

    switch (i)
    {
      case 0:
        pEditBox = pDlg_->GetDlgItem(IDC_CC1_NUMBER);
        ((CComboBox*)pDlg_->GetDlgItem(IDC_CC1_TYPE))->SelectString(-1, cctype);
        ((CComboBox*)pDlg_->GetDlgItem(IDC_CC1_EXPRMONTH))->SelectString(-1, expmo);
        ((CComboBox*)pDlg_->GetDlgItem(IDC_CC1_EXPRYEAR))->SelectString(-1, expyr);
        pDlg_->cc1_number_ = s;
        pDlg_->cc1_type_ = cctype;
        pDlg_->cc1_expmo_ = expmo;
        pDlg_->cc1_expyr_ = expyr;
        break;

      case 1:
        pEditBox = pDlg_->GetDlgItem(IDC_CC2_NUMBER);
        ((CComboBox*)pDlg_->GetDlgItem(IDC_CC2_TYPE))->SelectString(-1, cctype);
        ((CComboBox*)pDlg_->GetDlgItem(IDC_CC2_EXPRMONTH))->SelectString(-1, expmo);
        ((CComboBox*)pDlg_->GetDlgItem(IDC_CC2_EXPRYEAR))->SelectString(-1, expyr);
        pDlg_->cc2_number_ = s;
        pDlg_->cc2_type_ = cctype;
        pDlg_->cc2_expmo_ = expmo;
        pDlg_->cc2_expyr_ = expyr;
        break;

      case 2:
        pEditBox = pDlg_->GetDlgItem(IDC_CC3_NUMBER);
        ((CComboBox*)pDlg_->GetDlgItem(IDC_CC3_TYPE))->SelectString(-1, cctype);
        ((CComboBox*)pDlg_->GetDlgItem(IDC_CC3_EXPRMONTH))->SelectString(-1, expmo);
        ((CComboBox*)pDlg_->GetDlgItem(IDC_CC3_EXPRYEAR))->SelectString(-1, expyr);
        pDlg_->cc3_number_ = s;
        pDlg_->cc3_type_ = cctype;
        pDlg_->cc3_expmo_ = expmo;
        pDlg_->cc3_expyr_ = expyr;
        break;

      case 3:
        pEditBox = pDlg_->GetDlgItem(IDC_CC4_NUMBER);
        ((CComboBox*)pDlg_->GetDlgItem(IDC_CC4_TYPE))->SelectString(-1, cctype);
        ((CComboBox*)pDlg_->GetDlgItem(IDC_CC4_EXPRMONTH))->SelectString(-1, expmo);
        ((CComboBox*)pDlg_->GetDlgItem(IDC_CC4_EXPRYEAR))->SelectString(-1, expyr);
        pDlg_->cc4_number_ = s;
        pDlg_->cc4_type_ = cctype;
        pDlg_->cc4_expmo_ = expmo;
        pDlg_->cc4_expyr_ = expyr;
        break;
    }

    if (pEditBox)
    {
      pEditBox->SetWindowText(s);
    }
  }

  pDlg_->SetWindowText("Credit Cards");
  pDlg_->enableControls(true);


  for (; i < 4; i++)
  {
    switch (i)
    {
      case 0:
        pDlg_->GetDlgItem(IDC_CC1_TYPE)->GetWindowText(pDlg_->cc1_type_);
        pDlg_->GetDlgItem(IDC_CC1_EXPRMONTH)->GetWindowText(pDlg_->cc1_expmo_);
        pDlg_->GetDlgItem(IDC_CC1_EXPRYEAR)->GetWindowText(pDlg_->cc1_expyr_);
        break;

      case 1:
        pDlg_->GetDlgItem(IDC_CC2_TYPE)->GetWindowText(pDlg_->cc2_type_);
        pDlg_->GetDlgItem(IDC_CC2_EXPRMONTH)->GetWindowText(pDlg_->cc2_expmo_);
        pDlg_->GetDlgItem(IDC_CC2_EXPRYEAR)->GetWindowText(pDlg_->cc2_expyr_);
        break;

      case 2:
        pDlg_->GetDlgItem(IDC_CC3_TYPE)->GetWindowText(pDlg_->cc3_type_);
        pDlg_->GetDlgItem(IDC_CC3_EXPRMONTH)->GetWindowText(pDlg_->cc3_expmo_);
        pDlg_->GetDlgItem(IDC_CC3_EXPRYEAR)->GetWindowText(pDlg_->cc3_expyr_);
        break;

      case 3:
        pDlg_->GetDlgItem(IDC_CC4_TYPE)->GetWindowText(pDlg_->cc4_type_);
        pDlg_->GetDlgItem(IDC_CC4_EXPRMONTH)->GetWindowText(pDlg_->cc4_expmo_);
        pDlg_->GetDlgItem(IDC_CC4_EXPRYEAR)->GetWindowText(pDlg_->cc4_expyr_);
        break;
    }
  }
}


void CCreditCardDlg::AddCreditCardTypes(const std::list<std::string>& cctypes)
{
  cctypes_.resize(cctypes.size());

  int i = 0;
  for (std::list<std::string>::const_iterator it = cctypes.begin(),
                                              end = cctypes.end();
       it != end;
       ++it, ++i)
  {
    cctypes_[i] = (*it);
  }

  if (cctypes_.size() > 0)
  {
    int numTypes = cctypes_.size();

    ((CComboBox*)pDlg_->GetDlgItem(IDC_CC1_TYPE))->InitStorage(numTypes, 10);
    ((CComboBox*)pDlg_->GetDlgItem(IDC_CC2_TYPE))->InitStorage(numTypes, 10);
    ((CComboBox*)pDlg_->GetDlgItem(IDC_CC3_TYPE))->InitStorage(numTypes, 10);
    ((CComboBox*)pDlg_->GetDlgItem(IDC_CC4_TYPE))->InitStorage(numTypes, 10);

    for (int i = 0; i < numTypes; i++)
    {
      CString cctype = cctypes_[i].c_str();

      ((CComboBox*)pDlg_->GetDlgItem(IDC_CC1_TYPE))->AddString(cctype);
	  ((CComboBox*)pDlg_->GetDlgItem(IDC_CC2_TYPE))->AddString(cctype);
      ((CComboBox*)pDlg_->GetDlgItem(IDC_CC3_TYPE))->AddString(cctype);
      ((CComboBox*)pDlg_->GetDlgItem(IDC_CC4_TYPE))->AddString(cctype);  
    }

    ((CComboBox*)pDlg_->GetDlgItem(IDC_CC1_TYPE))->SetCurSel(0);
    ((CComboBox*)pDlg_->GetDlgItem(IDC_CC2_TYPE))->SetCurSel(0);
    ((CComboBox*)pDlg_->GetDlgItem(IDC_CC3_TYPE))->SetCurSel(0);
    ((CComboBox*)pDlg_->GetDlgItem(IDC_CC4_TYPE))->SetCurSel(0);
  }
}


void CCreditCardDlg::enableControls(bool b)
{
  GetDlgItem(IDC_CC1_NUMBER)->EnableWindow(b);
  GetDlgItem(IDC_CC2_NUMBER)->EnableWindow(b);
  GetDlgItem(IDC_CC3_NUMBER)->EnableWindow(b);
  GetDlgItem(IDC_CC4_NUMBER)->EnableWindow(b);

  GetDlgItem(IDC_CC1_TYPE)->EnableWindow(b);
  GetDlgItem(IDC_CC2_TYPE)->EnableWindow(b);
  GetDlgItem(IDC_CC3_TYPE)->EnableWindow(b);
  GetDlgItem(IDC_CC4_TYPE)->EnableWindow(b);

  GetDlgItem(IDC_CC1_EXPRMONTH)->EnableWindow(b);
  GetDlgItem(IDC_CC2_EXPRMONTH)->EnableWindow(b);
  GetDlgItem(IDC_CC3_EXPRMONTH)->EnableWindow(b);
  GetDlgItem(IDC_CC4_EXPRMONTH)->EnableWindow(b);

  GetDlgItem(IDC_CC1_EXPRYEAR)->EnableWindow(b);
  GetDlgItem(IDC_CC1_EXPRYEAR)->EnableWindow(b);
  GetDlgItem(IDC_CC1_EXPRYEAR)->EnableWindow(b);
  GetDlgItem(IDC_CC1_EXPRYEAR)->EnableWindow(b);
}

void CCreditCardDlg::OnOK() 
{
  CString prompt;

  CString cc1number, cc2number, cc3number, cc4number;
  GetDlgItem(IDC_CC1_NUMBER)->GetWindowText(cc1number);
  GetDlgItem(IDC_CC2_NUMBER)->GetWindowText(cc2number);
  GetDlgItem(IDC_CC3_NUMBER)->GetWindowText(cc3number);
  GetDlgItem(IDC_CC4_NUMBER)->GetWindowText(cc4number);

  CString nl("\r\n"), tab("\t"), space(" ");
  CString cc1type, cc1expmo, cc1expyr,
          cc2type, cc2expmo, cc2expyr,
          cc3type, cc3expmo, cc3expyr,
          cc4type, cc4expmo, cc4expyr;

  if (cc1number.GetLength() == 0 &&
      cc2number.GetLength() == 0 &&
      cc3number.GetLength() == 0 &&
      cc4number.GetLength() == 0)
  {
    prompt = CString("No credit cards will be saved to your account.\r\n");
  }
  else
  {
#if CONFIRM_CREDITCARD_INPUT
    CString start("The following credit cards will be saved to your account:\r\n\r\n");

    if (cc1number.GetLength() > 0)
    {
      GetDlgItem(IDC_CC1_TYPE)->GetWindowText(cc1type);
      GetDlgItem(IDC_CC1_EXPRMONTH)->GetWindowText(cc1expmo);
      GetDlgItem(IDC_CC1_EXPRYEAR)->GetWindowText(cc1expyr);

      prompt.Format("%s%16s %16s %4s %4s\r\n\r\n%16s %16s %4s %4s",
                    start, "Type", "Number", "Year", "Month",
                    cc1type, cc1number, cc1expyr, cc1expmo);
    }

    if (cc2number.GetLength() > 0)
    {
      GetDlgItem(IDC_CC2_TYPE)->GetWindowText(cc2type);
      GetDlgItem(IDC_CC2_EXPRMONTH)->GetWindowText(cc2expmo);
      GetDlgItem(IDC_CC2_EXPRYEAR)->GetWindowText(cc2expyr);
  
      prompt += cc2type + space +
                cc2number + space +
                cc2expyr + space +
                cc2expmo + nl;
    }

    if (cc3number.GetLength() > 0)
    {
      GetDlgItem(IDC_CC3_TYPE)->GetWindowText(cc3type);
      GetDlgItem(IDC_CC3_EXPRMONTH)->GetWindowText(cc3expmo);
      GetDlgItem(IDC_CC3_EXPRYEAR)->GetWindowText(cc3expyr);
  
      prompt += cc3type + space +
                cc3number + space +              
                cc3expyr + space +
                cc3expmo + nl;
    }

    if (cc4number.GetLength() > 0)
    {
      GetDlgItem(IDC_CC4_TYPE)->GetWindowText(cc4type);
      GetDlgItem(IDC_CC4_EXPRMONTH)->GetWindowText(cc4expmo);
      GetDlgItem(IDC_CC4_EXPRYEAR)->GetWindowText(cc4expyr);

      prompt += cc4type + space +
                cc4number + space +              
                cc4expyr + space +
                cc4expmo + nl;
    }
  }

  prompt += "\r\nIs this correct?";
  
  UINT action = AfxMessageBox(prompt, MB_YESNO|MB_ICONINFORMATION);
#else

    GetDlgItem(IDC_CC1_TYPE)->GetWindowText(cc1type);
    GetDlgItem(IDC_CC1_EXPRMONTH)->GetWindowText(cc1expmo);
    GetDlgItem(IDC_CC1_EXPRYEAR)->GetWindowText(cc1expyr);
    GetDlgItem(IDC_CC2_TYPE)->GetWindowText(cc2type);
    GetDlgItem(IDC_CC2_EXPRMONTH)->GetWindowText(cc2expmo);
    GetDlgItem(IDC_CC2_EXPRYEAR)->GetWindowText(cc2expyr);
    GetDlgItem(IDC_CC3_TYPE)->GetWindowText(cc3type);
    GetDlgItem(IDC_CC3_EXPRMONTH)->GetWindowText(cc3expmo);
    GetDlgItem(IDC_CC3_EXPRYEAR)->GetWindowText(cc3expyr);
    GetDlgItem(IDC_CC4_TYPE)->GetWindowText(cc4type);
    GetDlgItem(IDC_CC4_EXPRMONTH)->GetWindowText(cc4expmo);
    GetDlgItem(IDC_CC4_EXPRYEAR)->GetWindowText(cc4expyr);

#endif // #if CONFIRM_CREDITCARD_INPUT
  }

  UINT action = IDYES;
  if (action == IDYES)
  {
    using Network::CreditCardData;

    std::list<CreditCardData> ccards;

    if (cc1number.GetLength() > 0)
    {
      ccards.push_back(CreditCardData(cc1type,
                                      cc1number,
                                      cc1expmo,
                                      cc1expyr));
    }

    if (cc2number.GetLength() > 0)
    {
      ccards.push_back(CreditCardData(cc2type,
                                      cc2number,
                                      cc2expmo,
                                      cc2expyr));
    }

    if (cc3number.GetLength() > 0)
    {
      ccards.push_back(CreditCardData(cc3type,
                                      cc3number,
                                      cc3expmo,
                                      cc3expyr));
    }

    if (cc4number.GetLength() > 0)
    {
      ccards.push_back(CreditCardData(cc4type,
                                      cc4number,
                                      cc4expmo,
                                      cc4expyr));
    }

    if (Global::LoungeServer())
    {
      Global::LoungeServer()->sendCCardEditPDU(ccards);
    }

    CDialog::OnOK();
  }
}

void CCreditCardDlg::OnCancel() 
{
  // Check if user has done modifications,
  // if so confirm quit

  CString cc1number, cc2number, cc3number, cc4number;
  GetDlgItem(IDC_CC1_NUMBER)->GetWindowText(cc1number);
  GetDlgItem(IDC_CC2_NUMBER)->GetWindowText(cc2number);
  GetDlgItem(IDC_CC3_NUMBER)->GetWindowText(cc3number);
  GetDlgItem(IDC_CC4_NUMBER)->GetWindowText(cc4number);
  CString cc1type, cc1expmo, cc1expyr,
          cc2type, cc2expmo, cc2expyr,
          cc3type, cc3expmo, cc3expyr,
          cc4type, cc4expmo, cc4expyr;

  GetDlgItem(IDC_CC1_TYPE)->GetWindowText(cc1type);
  GetDlgItem(IDC_CC1_EXPRMONTH)->GetWindowText(cc1expmo);
  GetDlgItem(IDC_CC1_EXPRYEAR)->GetWindowText(cc1expyr);

  GetDlgItem(IDC_CC2_TYPE)->GetWindowText(cc2type);
  GetDlgItem(IDC_CC2_EXPRMONTH)->GetWindowText(cc2expmo);
  GetDlgItem(IDC_CC2_EXPRYEAR)->GetWindowText(cc2expyr);
  
  GetDlgItem(IDC_CC3_TYPE)->GetWindowText(cc3type);
  GetDlgItem(IDC_CC3_EXPRMONTH)->GetWindowText(cc3expmo);
  GetDlgItem(IDC_CC3_EXPRYEAR)->GetWindowText(cc3expyr);
  
  GetDlgItem(IDC_CC4_TYPE)->GetWindowText(cc4type);
  GetDlgItem(IDC_CC4_EXPRMONTH)->GetWindowText(cc4expmo);
  GetDlgItem(IDC_CC4_EXPRYEAR)->GetWindowText(cc4expyr);

  if (cc1number != cc1_number_ ||       
      cc2number != cc2_number_ ||
      cc3number != cc3_number_ ||
      cc4number != cc4_number_ ||

      cc1type != cc1_type_ ||
      cc2type != cc2_type_ ||
      cc3type != cc3_type_ ||
      cc4type != cc4_type_ ||

      cc1expmo != cc1_expmo_ ||
      cc2expmo != cc2_expmo_ ||
      cc3expmo != cc3_expmo_ ||
      cc4expmo != cc4_expmo_ ||

      cc1expyr != cc1_expyr_ ||
      cc2expyr != cc2_expyr_ ||
      cc3expyr != cc3_expyr_ ||
      cc4expyr != cc4_expyr_)
  {
    UINT action = AfxMessageBox("Exit without saving changes?", MB_YESNO|MB_ICONINFORMATION);
    if (action != IDYES)
      return;
  }

  CDialog::OnCancel();
}


u_int16_t CCreditCardDlg::GetCCTypeIndex(const CString& typeName)
{
  for (int i = 0; i < cctypes_.size(); i++)
  {
    if (typeName == cctypes_[i].c_str())
      return i;
  }

  return 0;
}
