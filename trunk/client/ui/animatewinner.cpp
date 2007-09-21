/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : namespace AnimateWinner
  Created  : 02/08/2000

  OVERVIEW : Implements the winner animation.

 ****************************************************************************/

#include "stdafx.h"
#include "base/transparentbitmap.h"
#include "base/misc.h"
#include "ui/resource.h"
#include "ui/global.h"
#include "ui/soundmanager.h"
#include <list>
using namespace std;

struct Winner
{
    CRect playerArea_;

    Winner()
        : playerArea_(0, 0, 0, 0)
    {}
    Winner(const CRect& r)
        : playerArea_(r)
    {}
    Winner(const Winner& w)
        : playerArea_(w.playerArea_)
    {}
    Winner& operator = (const Winner& rhs)
    {
        if (this != &rhs)
        {
            playerArea_ = rhs.playerArea_;
        }
        return *this;
    }
};

// There can be multiple winners, collect
// areas into a list for animation
typedef list<Winner> Winners;
typedef Winners::iterator WIT;
static Winners g_winners;

namespace
{


CWnd* g_pWnd = 0;
UINT  g_Timer = 0;
DWORD g_dwAnimTime = 0, g_dwDT = 0;
BOOL  g_bFlipFlop = TRUE;
Base::TransparentBitmap g_Bmp;

CRect GetLeftRect(const CRect& rect, const CSize& size)
{
  CPoint pt = rect.TopLeft() - CPoint(2, 0);
  CRect r(pt, size);
  Base::CenterRect(r, rect, DT_VCENTER);
  return r;
}

CRect GetRightRect(const CRect& rect, const CSize& size)
{
  CPoint pt = rect.BottomRight() + CPoint(2, 0);
  CRect r(pt, size);
  r.OffsetRect(-size.cx, 0);
  Base::CenterRect(r, rect, DT_VCENTER);
  return r;
}

VOID CALLBACK AnimProc(HWND hwnd,     
                       UINT uMsg,     
                       UINT idEvent,  
                       DWORD dwTime)
{
  if (!g_pWnd) return;

  bool isOffline = (Global::TableServer() == NULL);

  static DWORD prevTime = 0;
  if (prevTime == 0)
    prevTime = ::timeGetTime();
  
  g_dwDT += (timeGetTime() - prevTime);
  if (isOffline || g_dwDT > g_dwAnimTime)
  { // Animation is done
    ::KillTimer(NULL, idEvent);
    g_Timer = g_dwDT = prevTime = 0;
    g_bFlipFlop = TRUE;

    // Clear out last images just in case
    for (WIT it = g_winners.begin(),
             end = g_winners.end();
         it != end;
         ++it)
    {
      CRect r = GetLeftRect((*it).playerArea_, g_Bmp.GetSize());
      g_pWnd->InvalidateRect(&r);

      r = GetRightRect((*it).playerArea_, g_Bmp.GetSize());
      g_pWnd->InvalidateRect(&r);
    }

    // empty the list
    g_winners.clear();

    if (!isOffline)
    { // Prepare for next hand
      SndMan::Instance()->playSound(SndMan::SM_Shuffle);
    }

    return;
  }

  prevTime = ::timeGetTime();

  if (g_bFlipFlop)
  {
    CDC* pDC = g_pWnd->GetDC();
    if (pDC)
    {
      for (WIT it = g_winners.begin(),
               end = g_winners.end();
           it != end;
           ++it)
      {
        CRect r = GetLeftRect((*it).playerArea_, g_Bmp.GetSize());
        g_Bmp.TransBlt(pDC, r.left, r.top);

        r = GetRightRect((*it).playerArea_, g_Bmp.GetSize());
        g_Bmp.TransBlt(pDC, r.left, r.top);
      }

      g_pWnd->ReleaseDC(pDC);
      pDC = NULL;
    }
  }
  else
  {
    for (WIT it = g_winners.begin(),
             end = g_winners.end();
         it != end;
         ++it)
    {
      CRect r = GetLeftRect((*it).playerArea_, g_Bmp.GetSize());
      g_pWnd->InvalidateRect(&r);

      r = GetRightRect((*it).playerArea_, g_Bmp.GetSize());
      g_pWnd->InvalidateRect(&r);
    }
  }

  g_bFlipFlop = !g_bFlipFlop;
}
 
}

namespace AnimateWinner
{

BOOL StartAnimate(CWnd* pWnd, const CRect& playerArea, UINT msecs)
{
  if (g_Bmp.GetSafeHandle() == NULL)
  {
    BOOL rc = g_Bmp.LoadBitmap(IDB_DOLLARIMAGE, IDB_DOLLARMASK);
    ASSERT(rc);
  }

  if (g_Timer != 0)
    ::KillTimer(NULL, g_Timer);

  g_pWnd = pWnd;
  CRect r(playerArea);
  // Make rect larger so the bitmap can be painted
  // out of the original rect's borders
  r.InflateRect(g_Bmp.GetSize().cx, 0);
  g_winners.push_back(Winner(r));
  g_dwAnimTime = msecs;
  g_bFlipFlop = TRUE;

  // "Animate" twice per second
  g_Timer = ::SetTimer(NULL, 0, 500, AnimProc);

  return g_Timer != 0;
}


BOOL StopAnimate()
{
  g_winners.clear();
  return TRUE;
}

}

