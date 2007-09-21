/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduReseatAck
  Created  : 07/26/2000

  OVERVIEW : Receive-only tournament reseat ack pdu.
             Table server tells lounge server that a player
             reseat was successfull.

 ****************************************************************************/

#include "tournament/pdureseatack.h"
#include "tournament/basetournamentqueue.h"
//#include "tournament/tournament.h"
#include "tournament/pdureseatplayer.h"
#include "lounge/lounge.h"
#include "pdu/pdusitting.h"
#include "pdu/pduqueuefull.h"
#include "sys.h"

REGISTER_INCOMING_PDU(PDU_Tournament_ReseatAck, CpduReseatAck);


CpduReseatAck::CpduReseatAck()
{
  memset(sz_username, 0, sizeof(sz_username));
}

void CpduReseatAck::grok(SOCKET sd, CTable* table)
{
  // Copy username zero-terminated string
  pdu_reseatack* data = (pdu_reseatack*)(value_ + PDU_HEADERSIZE);
  memset(sz_username, 0, sizeof(sz_username));
  memcpy(sz_username, data->username, PDU_STRINGSIZE);
  
  printf("CpduReseatAck %s\n", sz_username);
  
  // Now actually reseat the player
  bool okToReseat = true;
  
  CBaseTournamentQueue* tq = 
    CBaseTournamentQueue::FindQueue(table);

  if (tq)
  {
  
  CTable* oldTable = tq->findTableWithPlayer(sz_username);
  if (!oldTable)
  {
    okToReseat = false;
    char s[300];
    sprintf(s, "CpduReseatAck: table for %s not found", sz_username);
    Sys_LogError(s);
  }
  else
  {
    if (oldTable == table)
    { // The player is being seated at the same table he
      // is already playing at - zombie is being reanimated.
      
      CPlayer* player = CLounge::Inst()->findPlayer(sz_username);
      // 03/09/2001 Tournament table dissolve bug fix.
      // While a tournament table merge is in progress, the player
      // must not be allowed to sit back at his table because that
      // could interfere with the merge process causing the dissolve
      // to fail and ultimately hang the whole tournament!
      if (tq->tableDissolveInProgress(table))
      {
        okToReseat = false;
        char s[300];
        sprintf(s, "CpduReseatAck: zombie %s reseat while table dissolved.", sz_username);
        Sys_LogError(s);
        
        if (player)
        {
          CpduQueueFull pdu;
          pdu.sendQueueFull(tq->getQueueNumber(),
                            player,
                            QF_TournamentSeating);
        }
      }
      else
      {
        if (!player)
        {
          char s[100];
          sprintf(s, "CpduReseatAck: %s not found\n", sz_username);
          Sys_LogError(s);
        }
        else
        {
          CpduSitting pdu;
          pdu.sendSitting(player,
                          table->getNumber(),
                          table->getPort(),
                          table->getAddr());
        }
      }
    }
    else
    { // Tournament table is merged and player is reseated
      // at another table.
      CpduReseatPlayer pdu;
      pdu.sendReseatPlayer(sz_username, table, oldTable);
    }
    
    if (okToReseat)
    {
      // Update the player list in the table
      CLounge::Inst()->playerSeated(sz_username, table);
    }
  }

  }
  else
  {
    Sys_LogError("ERROR: No tournament queue found");
  }
  
  // this pdu is done
  delete this;
}
