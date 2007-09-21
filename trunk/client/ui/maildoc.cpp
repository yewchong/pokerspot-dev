#include "stdafx.h"
#include "maildoc.h"
#include <mapi.h>

// The purpose of this CDocument subclass is to provide the
// application with email support

static BOOL g_bIsMailAvail = (BOOL)-1;    // start out not determined

class MailState
{
public:
    MailState() : m_hInstMail(0) {}
	~MailState()
    {
	  if (m_hInstMail != NULL)
	    ::FreeLibrary(m_hInstMail);
      m_hInstMail = NULL;
    }

	HINSTANCE m_hInstMail;      // handle to MAPI32.DLL
};

static MailState g_MailState;


CMailDocument::CMailDocument(const CString& addr, const CString& text)
  :
  address_(addr),
  body_   (text)
{}

CMailDocument::~CMailDocument()
{}

void CMailDocument::setBody(const CString& s)
{
  body_ = s;
}

void CMailDocument::setSubject(const CString& s)
{
  subject_ = s;
}


void CMailDocument::setAddress(const CString& s)
{
  address_ = s;
}

BOOL CMailDocument::IsMAPIAvailable()
{
  if (g_bIsMailAvail == (BOOL)-1)
  {
    g_bIsMailAvail = ::GetProfileInt(_T("MAIL"), _T("MAPI"), 0) != 0;
    if (g_bIsMailAvail)
      g_bIsMailAvail = SearchPath(NULL, _T("MAPI32.DLL"), NULL, 0, NULL, NULL) != 0;
  }

  return g_bIsMailAvail;
}

BOOL CMailDocument::sendMail(bool showUI)
{
  if (!g_bIsMailAvail) return FALSE;

	CWaitCursor wait;

	MailState* pMailState = &g_MailState;
	if (pMailState->m_hInstMail == NULL)
		pMailState->m_hInstMail = ::LoadLibraryA("MAPI32.DLL");

	if (pMailState->m_hInstMail == NULL)
	{
		AfxMessageBox(AFX_IDP_FAILED_MAPI_LOAD);
		return FALSE;
	}
	ASSERT(pMailState->m_hInstMail != NULL);

	ULONG (PASCAL *lpfnSendMail)(ULONG, ULONG, MapiMessage*, FLAGS, ULONG);
	(FARPROC&)lpfnSendMail = GetProcAddress(pMailState->m_hInstMail, "MAPISendMail");
	if (lpfnSendMail == NULL)
	{
		AfxMessageBox(AFX_IDP_INVALID_MAPI_DLL);
		return FALSE;
	}
	ASSERT(lpfnSendMail != NULL);

	// prepare the message (empty with 1 attachment)
	MapiMessage message;
	memset(&message, 0, sizeof(message));

    if (body_.GetLength() > 0)
      message.lpszNoteText = (LPSTR)(LPCSTR)body_;
    if (subject_.GetLength() > 0)
      message.lpszSubject = (LPSTR)(LPCSTR)subject_;

    MapiRecipDesc recip;
    memset(&recip, 0, sizeof(recip));
    recip.ulRecipClass = MAPI_TO;
    recip.lpszName = (LPSTR)(LPCSTR)address_;

    message.nRecipCount = 1;
    message.lpRecips = &recip;

	HWND hWndTop;
	CWnd* pParentWnd = CWnd::GetSafeOwner(NULL, &hWndTop);

	// prepare for modal dialog box
	AfxGetApp()->EnableModeless(FALSE);

	// some extra precautions are required to use MAPISendMail as it
	// tends to enable the parent window in between dialogs (after
	// the login dialog, but before the send note dialog).
	pParentWnd->SetCapture();
	::SetFocus(NULL);
	pParentWnd->m_nFlags |= WF_STAYDISABLED;

    DWORD flags = MAPI_LOGON_UI;
    if (showUI)
      flags |= MAPI_DIALOG;

    int nError = lpfnSendMail(0, (ULONG)pParentWnd->GetSafeHwnd(),
		                      &message, flags, 0);

	// after returning from the MAPISendMail call, the window must
	// be re-enabled and focus returned to the frame to undo the workaround
	// done before the MAPI call.
	::ReleaseCapture();
	pParentWnd->m_nFlags &= ~WF_STAYDISABLED;

	pParentWnd->EnableWindow(TRUE);
	::SetActiveWindow(NULL);
	pParentWnd->SetActiveWindow();
	pParentWnd->SetFocus();
	if (hWndTop != NULL)
		::EnableWindow(hWndTop, TRUE);
	AfxGetApp()->EnableModeless(TRUE);

	if (nError != SUCCESS_SUCCESS &&
		nError != MAPI_USER_ABORT && nError != MAPI_E_LOGIN_FAILURE)
	{
		AfxMessageBox(AFX_IDP_FAILED_MAPI_SEND);
	}


  return TRUE;
}

