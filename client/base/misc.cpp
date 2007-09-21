/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : misc.cpp
  Created  : 31.10.1999

  OVERVIEW : Misc helper routines.

 ****************************************************************************/

#include "stdafx.h"
#include "base/misc.h"
#include "base/card.h"
#include "ui/mainwnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace Base
{

void SafeRemove(const char* fname)
{
  TRY
  {
    CFile::Remove(fname);
  }
  CATCH (CFileException, e)
  {
  }
  END_CATCH
}

int PointsToPixels(int points)
{
  CDC dc;
  dc.CreateCompatibleDC(NULL);
  return -((points * GetDeviceCaps(dc.GetSafeHdc(), LOGPIXELSY)) / 72);
}

BOOL IsMaximized(CWnd* pWnd)
{
  WINDOWPLACEMENT wpml;
  ::ZeroMemory(&wpml, sizeof(wpml));
  wpml.length = sizeof(wpml);

  pWnd->GetWindowPlacement(&wpml);
  return wpml.showCmd == SW_MAXIMIZE;
}


CView* GetActiveView()
{
  CView* pView = NULL;

#ifdef PSPOT_TABLE_MODULE_
  CMainWnd* pMainWnd = CMainWnd::Instance();

  if (pMainWnd)
    pView = pMainWnd->GetActiveView();
#endif

  return pView;
}

void SanityCheckMinMax(DWORD& x, DWORD minx, DWORD maxx)
{
  x = max(x, minx);
  x = min(x, maxx);
}

void CenterRect(CRect& r1, const CRect& r2, DWORD flags)
{
  if (flags & DT_CENTER)
  { // center horizontally
    int w = r1.Width();
    r1.left = r2.left + r2.Width() / 2 - r1.Width() / 2;
    r1.right = r1.left + w;
  } 

  if (flags & DT_VCENTER)
  { // center vertically
    int h = r1.Height();
    r1.top = r2.top + r2.Height() / 2 - r1.Height() / 2;
    r1.bottom = r1.top + h;
  }
}


void SetStatusText(CWnd*, LPCTSTR text)
{
#ifdef PSPOT_TABLE_MODULE_
  CMainWnd::SetStatusText(text);
#endif
}


char SuitToChar(Base::Suit s)
{
  switch (s)
  {
    case Base::Clubs:
      return 'c';
      break;

    case Base::Diamonds:
      return 'd';
      break;

    case Base::Hearts:
      return 'h';
      break;

    case Base::Spades:
      return 's';
      break;

    default:
      ASSERT(FALSE);
      return 'X';
  }
}


char ValueToChar(int v)
{
  if (v > 1 && v < 10)
    return v + '0';
  else if (v == 10)
    return 'T';
  else if (v == 11)
    return 'J';
  else if (v == 12)
    return 'Q';
  else if (v == 13)
    return 'K';
  else if (v == 14)
    return 'A';
  else
  {
    ASSERT(FALSE);
    return '0';
  }
}
  

CString AsString(const Card& card)
{
  if (card.number() == 0 || card.suit() == 0)
    return CString("b"); // face down card

  CString s;
  s.Format("%c%c", ValueToChar(card.number()), SuitToChar(card.suit()));
  return s;
}


}