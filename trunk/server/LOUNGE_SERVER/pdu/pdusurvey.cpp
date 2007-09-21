/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduSurvey
  Created  : 04/18/2000

  OVERVIEW : Lounge survey PDU.

 ****************************************************************************/

#include "pdu/pdusurvey.h"
#include "pdu/pdustatus.h"
#include "pdu/pduqueueupdate.h"

REGISTER_INCOMING_PDU(PDU_Lounge_Survey, CpduSurvey);

CpduSurvey::CpduSurvey()
{}

void CpduSurvey::grok(SOCKET sd, CPlayer* player)
{
  // Send game names
  CpduStatus s1;
  s1.sendGameNames(sd);
  // Send waiting lists
  CpduStatus s2;
  s2.sendTo(sd);
  
  // Send full waiting list state update
  CpduQueueUpdate update;
  update.sendFullUpdate(player);
  
  
  // this pdu is done
  delete this;
}
