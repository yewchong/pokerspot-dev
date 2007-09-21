/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDUQueueUpdate
  Created  : 03/13/2000

  OVERVIEW : Lounge server sends this PDU to the lounge client
             to notify a change in the queue state. The change
             can be either player add/remove or table add/remove
             event.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pduqueueupdate.h"
#include "loungedlg/loungedata.h"
#include "loungedlg/resource.h"
#include "loungedlg/loungedlg.h"


BEGIN_NAMESPACE(Network)


void PDUQueueUpdate2::execute(GameState*)
{
#ifndef PSPOT_TABLE_MODULE_

  CLoungeDlg* pDlg = static_cast<CLoungeDlg*>(AfxGetMainWnd());
  if (pDlg)
  {
    QueueDelta* pD = pDeltas_;
    for (int i = 0; i < numActions_; i++, pD++)
    {
      Lounge::Queue& queue = Lounge::GetQueue(pD->queue_);
      if (Lounge::IsValid(queue))
      {
        string s((LPCSTR)MakeString(pD->name_));

        if (pD->action_ == Action_Player_Remove ||
            pD->action_ == Action_Player_Serviced)
        {
          Lounge::RemovePlayerFromQueue(queue, s);
        }
        else if (pD->action_ == Action_Player_Add)
        {
          Lounge::AddPlayerToQueue(queue, s);
        }
        else if (pD->action_ == Action_Table_Add)
        {
          Lounge::AddTableToQueue(queue, s);
        }
        else if (pD->action_ == Action_Table_Remove)
        {
          Lounge::RemoveTableFromQueue(queue, s);
        }
        else if (pD->action_ == Action_Tournament_Add)
        {
          // Tournament is interpreted as a table
          Lounge::AddTableToQueue(queue, s);
        }
        else if (pD->action_ == Action_Tournament_Remove)
        {
          Lounge::RemoveTableFromQueue(queue, s);
        }
        else
        {
          PDU_DEBUG("Queue Update PDU: Invalid queue update.");
        }
      }
    }
  }
#endif
}

void PDUQueueUpdate::execute(GameState*)
{
#ifndef PSPOT_TABLE_MODULE_
#endif
}


END_NAMESPACE(Network)

