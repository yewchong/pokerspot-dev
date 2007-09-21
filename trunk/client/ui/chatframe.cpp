/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CChatFrame
  Created  : 5.11.1999

  OVERVIEW : CChatFrame is the CFrameWnd used for CChatView.

 ****************************************************************************/

#include "stdafx.h"
#include "ui/chatframe.h"
#include "ui/chatview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChatFrame


CChatFrame::CChatFrame()
{
}

CChatFrame::~CChatFrame()
{
}


BEGIN_MESSAGE_MAP(CChatFrame, CFrameWnd)
  //{{AFX_MSG_MAP(CChatFrame)
    // NOTE - the ClassWizard will add and remove mapping macros here.
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChatFrame message handlers

BOOL CChatFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
  BOOL rc = FALSE;

  CChatView* pView = new CChatView();
  if (pView)
  {
    rc = pView->Create(NULL, NULL, WS_VISIBLE|WS_CHILD,
                       rectDefault, this, AFX_IDW_PANE_FIRST, pContext);
    if (rc)
    {
      pView->OnInitialUpdate();
      SetActiveView(pView);
    }
  }

  if (rc)
    return CFrameWnd::OnCreateClient(lpcs, pContext);
  else
    return FALSE;
}
