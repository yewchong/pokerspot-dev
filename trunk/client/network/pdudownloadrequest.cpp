/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDUDownloadRequest
  Created  : 01/30/2000

  OVERVIEW : Download Request PDU.

             When the lounge server detects that the
             client has old versions of files, it sends
             a Download Request PDU that tells the
             client where to download new versions of
             the outdated files.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pdudownloadrequest.h"
#include <atlbase.h>

BEGIN_NAMESPACE0()
USING_NS_T(Network, PDUDownloadRequest);

LPCTSTR g_szDefaultUpgExe = _T("upgradewizard.exe");
LPCTSTR g_szUpgradeKey = _T("Software\\PokerSpot International\\Upgrade");
LPCTSTR g_szDownloads = _T("Downloads");


BOOL AddDownloadRequest(int dlnum, 
                        const CString& module,
                        DWORD size,
                        const CString& url)
{
  CRegKey key;
  CString subkey(g_szUpgradeKey);
  subkey += CString("\\") + CString(g_szDownloads);
  CString dlkey;
  dlkey.Format("dl%d", dlnum);
  BOOL rc = (key.Create(HKEY_CURRENT_USER, subkey) == ERROR_SUCCESS);

  if (rc)
  { // Add the download values
    key.SetValue(module, "module");
    key.SetValue(url, "url");
    key.SetValue(size, "size");
  }

  key.Close();

  return rc;
}


void AddDownload(int number, PDUDownloadRequest::DownloadEntry* pD)
{
  CString module = reinterpret_cast<char*>(pD->module_);
  CString url = pD->url_;
  DWORD   size = pD->modulesize_;

  CString s;
  s.Format("   Download request: module: %s ftp: %s size: %d",
           (LPCTSTR)module, (LPCTSTR)url, size);
  PDU_DEBUG(s);

  if (!module.IsEmpty() && !url.IsEmpty())
    AddDownloadRequest(number, module, size, url);
}


// Destroy current download subkeys. This
// is done by destroying the Downloads key
// (which recursively destroys subkeys) and
// recreating it.
BOOL DestroyDownloads()
{
  BOOL rc = FALSE;

  CRegKey key;
  CString subkey(g_szUpgradeKey);
  if (key.Open(HKEY_CURRENT_USER, subkey) == ERROR_SUCCESS)
  {
    subkey += CString("\\") +
              CString(g_szDownloads);
    rc = (key.RecurseDeleteKey(subkey) == ERROR_SUCCESS);
    if (rc)
    {
      key.Close();
      // Recreate it
      rc = (key.Create(HKEY_CURRENT_USER, subkey) == ERROR_SUCCESS);
      key.Close();
    }
  }

  return rc;
}


END_NAMESPACE0()


BEGIN_NAMESPACE(Network)


//
// Go through download entries and save their
// data to registry for the Upgrade Wizard.
//
void PDUDownloadRequest::execute(GameState*)
{
  // Destroy all download request subkeys 
  DestroyDownloads();

  // Add new download request subkeys
  for (int i = 0; i < numdownloads_; i++)
  {
    AddDownload(i, &pDownloads_[i]);
  }

  // PostMessage(WM_COMMAND, MAKEWPARAM(ID_EXIT, 0), 0);

  // GetUpgradeWizard

  // Start the download
  //ShellExecute(upgWiz);
}

//
// Extract download entries from buffer.
//
void PDUDownloadRequest::extractDownloadEntries(u_int16_t* pint)
{
  numdownloads_ = ntohs(*pint++);
  u_byte_t* pbyte = (u_byte_t*)pint;

  if (numdownloads_ > 0)
    pDownloads_ = new DownloadEntry[numdownloads_];

  if (numdownloads_ > 0 && !pDownloads_)
    numdownloads_ = 0; // out of memory!
  else
  {
    DownloadEntry* pD = pDownloads_;
    for (int i = 0; i < numdownloads_; i++, pD++)
    {
      // Copy download entry from buffer
      memcpy(pD->module_, pbyte, MODULE_SIZE);
      pbyte += MODULE_SIZE;
      u_int32_t* plong = (u_int32_t*)pbyte;
      pD->modulesize_ = ntohl(*plong++);  
      pint = (u_int16_t*)plong;
      pD->urlsize_ = ntohs(*pint++);

      pbyte = (u_byte_t*)(pint);
      if (pD->urlsize_ > 0)
        pD->url_ = new char[pD->urlsize_ + 1];
      if (pD->url_)
      {
        memcpy(pD->url_, pbyte, pD->urlsize_);
        pD->url_[pD->urlsize_] = '\0';
      }
      pbyte += pD->urlsize_;
    }
  }
}
   
END_NAMESPACE(Network)
