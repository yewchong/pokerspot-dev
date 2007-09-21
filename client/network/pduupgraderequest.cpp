/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDUUpgradeRequest
  Created  : 01/30/2000

  OVERVIEW : Upgrade Request PDU.

             When the lounge server detects that the
             client has old versions of files, it sends
             an Upgrade Request PDU that contains the new
             client configuration file.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pduupgraderequest.h"
#include "loungedlg/resource.h"
#include "loungedlg/registry.h"

BEGIN_NAMESPACE(Network)


//
// Save the configuration data to file
// "client_versions.conf".
//
void PDUUpgradeRequest::execute(GameState*)
{
#ifndef PSPOT_TABLE_MODULE_
  const int MsgBufSize = 255;
  LPCTSTR filename = _T("client_versions.conf");

  CFileException fex;
  CFile f;
  if (f.Open(filename, CFile::modeCreate | CFile::modeWrite, &fex))
  {
    TRY
    {
      f.Write((LPCTSTR)config_data_, length_ - sizeOf());
      f.Close();
    }
    CATCH(CFileException, e)
    {
      TCHAR szCause[MsgBufSize];     
      e->GetErrorMessage(szCause, MsgBufSize);
      CString s;
      s.Format("Can't write file '%s': %s.", filename, szCause);
      AfxMessageBox(s);
    }
    END_CATCH
  }
  else
  {
    TCHAR szCause[MsgBufSize];     
    fex.GetErrorMessage(szCause, MsgBufSize);
    CString s;
    s.Format("Can't create file '%s': %s", filename, szCause);
    AfxMessageBox(s);
  }

  AfxMessageBox("A new version of the client is available.", MB_ICONINFORMATION);
 
  // Start the download
  CString upgWiz = Registry::GetUpgradeWizard("upgwiz2.exe");
  int rc = (int)ShellExecute(NULL, "open", upgWiz, NULL, NULL, SW_SHOWNORMAL);

  if (rc <= 32)
  {
    AfxMessageBox("Unable to run Upgrade Wizard.\n\nPlease reinstall PokerSpot client. "
                  "Automatic upgrade will bring your installation up to date the next time you log in.");
  }

  if (AfxGetMainWnd())
    AfxGetMainWnd()->PostMessage(WM_CLOSE);
#endif
}

   
END_NAMESPACE(Network)
