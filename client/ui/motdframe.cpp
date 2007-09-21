/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CMotdFrame
  Created  : 28.10.1999

  OVERVIEW : CFrameWnd derivative for the MOTD dialog.
      
             NOTE: In case the MOTD message is the contents
             instead of an URL, then the contents is saved
             to a temporary file and the HTML control is instructed
             to load the contents from that file. This is nasty
             but I couldn't find any other way to get the contents
             into the control.

 ****************************************************************************/

#include "stdafx.h"
#include "motdframe.h"
#include "motdview.h"
#include "base/misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
  const int MsgBufSize = 255;
}


/////////////////////////////////////////////////////////////////////////////
// CMotdFrame

CMotdFrame::CMotdFrame(LPCTSTR motdMessage)
  :
  motdMessage_(motdMessage)
{
}


CMotdFrame::~CMotdFrame()
{
  if (tmpFileName_.GetLength() > 0)
  {
    Base::SafeRemove(tmpFileName_);
  }
}


BEGIN_MESSAGE_MAP(CMotdFrame, CFrameWnd)
  //{{AFX_MSG_MAP(CMotdFrame)
  ON_WM_CREATE()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMotdFrame message handlers

BOOL CMotdFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
  BOOL rc = FALSE;

  CMotdView* pView = new CMotdView();
  if (pView)
  {
    rc = pView->Create(NULL, NULL, WS_VISIBLE|WS_CHILD,
                       rectDefault, this, AFX_IDW_PANE_FIRST, pContext);

    if (rc)
    {
#ifdef HTML_VIEW_
      if (isURL(motdMessage_))
        pView->Navigate2(motdMessage_);
      else
      { // save content to temporary file & show it
        tmpFileName_ = saveToTmpFile(motdMessage_);
        if (tmpFileName_.GetLength())
        {
          CString url(_T("file://"));
          url += tmpFileName_;
          pView->Navigate2(url);
        }
      }
#endif
    }
  }

  if (rc)
    rc = CFrameWnd::OnCreateClient(lpcs, pContext);

  return rc;
}

static UINT indicators[] =
{
	ID_SEPARATOR
};


int CMotdFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
    return -1;

#ifdef SHOW_MOTD_BROWSERSTATUSBAR_
  if (!statusBar_.Create(this) ||
      !statusBar_.SetIndicators(indicators,
                                sizeof(indicators)/sizeof(UINT)))
  {
    TRACE0("Failed to create status bar\n");
    return -1;      // fail to create
  }
#endif

  return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CMotdFrame application mfunction

bool CMotdFrame::isURL(const CString& s) const
{
  // Choose the easy path...
  CString urlStart("http://");
  CString smallCaps(s);
  smallCaps.MakeLower();
  return smallCaps.Find(urlStart) == 0;
}


//
// Save the data in 'data' to a temporary file,
// return the name of the file.
//
CString CMotdFrame::saveToTmpFile(const CString& data)
{
  CString name;
  static int c = 0;
  char tmpName[MAX_PATH];
  char tmpPath[MAX_PATH];

  ::GetTempPath(MAX_PATH, tmpPath);

  if (::GetTempFileName(tmpPath, "motd", c++, tmpName))
  {
    // Replace extension tmp with htm
    int l = lstrlen(tmpName);
    if (l > 3)
    {
      tmpName[l-1] = 'M';
      tmpName[l-2] = 'T';
      tmpName[l-3] = 'H';
    }

    CFileException fex;
    CFile f;
    if (f.Open(tmpName, CFile::modeCreate | CFile::modeWrite, &fex))
    {
      TRY
      {
        f.Write((LPCTSTR)data, data.GetLength());
        f.Close();
        name = tmpName;
      }
      CATCH(CFileException, e)
      {
        TCHAR szCause[MsgBufSize];     
        e->GetErrorMessage(szCause, MsgBufSize);

        CString s;
        s.Format("Can't write temporary file for showing Message of the Day: %s.",
                 szCause);
        GetParent()->MessageBox(s, "Error");
      }
      END_CATCH
    }
    else
    {
      TCHAR szCause[MsgBufSize];     
      fex.GetErrorMessage(szCause, MsgBufSize);
      CString s;
      s.Format("Can't create temporary file for showing Message of the Day: %s.",
               szCause);
      GetParent()->MessageBox(s, "Error");
    }
  }
  else
  {
    GetParent()->MessageBox("Can't create temporary file for showing Message of the Day",
                            "Error");
  }
    
  return name;
}

#ifdef NOT_TESTED_

HRESULT CMotdFrame::GetDocumentContent()
{
  USES_CONVERSION;
  
  // Get the WebBrowser's document object
  CComPtr<IDispatch> pDisp;
  HRESULT hr = m_spWebBrowser2->get_Document(&pDisp);
  if (FAILED(hr))
   return hr;

  // Verify that what we get is a pointer to a IHTMLDocument2 
  // interface. To be sure, let's query for 
  // the IHTMLDocument2 interface (through smart pointers)
  CComQIPtr<IHTMLDocument2, &IID_IHTMLDocument2> spHTML;
  spHTML = pDisp;

  // Extract the source code of the document
  if (spHTML)
  {
    // Get the BODY object
    hr = spHTML->get_body(&m_pBody); 
    if (FAILED(hr))
        return hr;

    // Get the HTML text
    BSTR bstrHTMLText;
    hr = m_pBody->get_outerHTML(&bstrHTMLText); 
    if (FAILED(hr))
     return hr;

    // Convert the text from Unicode to ANSI
    LPTSTR psz = new TCHAR[SysStringLen(bstrHTMLText)];
    lstrcpy(psz, OLE2T(bstrHTMLText));
      
    // Enable changes to the text
    HWND hwnd = m_dlgCode.GetDlgItem(IDC_TEXT);
    EnableWindow(hwnd, true);
    hwnd = m_dlgCode.GetDlgItem(IDC_APPLY);
    EnableWindow(hwnd, true);

    // Set the text in the Code Window
    m_dlgCode.SetDlgItemText(IDC_TEXT, psz); 
    delete [] psz;
  }
  else   // The document isn't a HTML page
  {
    m_dlgCode.SetDlgItemText(IDC_TEXT, ""); 
    HWND hwnd = m_dlgCode.GetDlgItem(IDC_TEXT);
    EnableWindow(hwnd, false);
    hwnd = m_dlgCode.GetDlgItem(IDC_APPLY);
    EnableWindow(hwnd, false);
  }

  return S_OK;  
}

#endif