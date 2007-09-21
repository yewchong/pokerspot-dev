/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CChatEdit
  Created  : 25.10.1999

  OVERVIEW : CChatEdit implements the edit box in CChatView.

 ****************************************************************************/

#include "stdafx.h"
#include "ui/chatedit.h"
#include "ui/chatview.h"
#include "ui/tableview.h"
#include "base/player.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChatEdit

IMPLEMENT_DYNAMIC(CChatEdit, CEdit)

CChatEdit::CChatEdit()
{
}


CChatEdit::~CChatEdit()
{
}


BEGIN_MESSAGE_MAP(CChatEdit, CEdit)
  //{{AFX_MSG_MAP(CChatEdit)
  // NOTE - the ClassWizard will add and remove mapping macros here.
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChatEdit message handlers
/////////////////////////////////////////////////////////////////////////////

