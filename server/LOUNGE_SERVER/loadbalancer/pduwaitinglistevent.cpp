/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduWaitinglistEvent
  Created  : 10/02/2001

  OVERVIEW : This PDU is used for sending waiting list events to
             other Lounge Servers.

             The possible waiting list events are:
             - table addition
             - table remove
             - tournament addition
             - tournament remove

 ****************************************************************************/

#include "loadbalancer/pduwaitinglistevent.h"
#include "lounge/table.h"
#include "lounge/lounge.h"
#include "tournament/tournament.h"


REGISTER_INCOMING_PDU(PDU_LB_WaitinglistEvent, CpduWaitinglistEvent);


CpduWaitinglistEvent::CpduWaitinglistEvent()
{
  type_ = PDU_LB_WaitinglistEvent;
  length_ = PDU_HEADER_SIZE;
  stuffHeader();
}

int CpduWaitinglistEvent::sendRemove(u_int16_t queuenumber,
                                     CTable* table)
{
  return sendDelta(queuenumber, table, WLE_Table_Remove);
}

int CpduWaitinglistEvent::sendAdd(u_int16_t queuenumber,
                                  CTable* table)
{
  return sendDelta(queuenumber, table, WLE_Table_Add);
}

int CpduWaitinglistEvent::sendDelta(u_int16_t queuenumber,
                                    CTable*   table,
                                    u_int16_t reason)
{
  length_ += sizeof(pdu_waitinglisteventheader) + sizeof(pdu_tableevent);
  stuffHeader();
  
  char* buf = value_ + PDU_HEADER_SIZE;
  
  pdu_waitinglisteventheader data;
  memset(&data, 0, sizeof(data));
  data.queueNumber = htons(queuenumber);
  data.reason = htons(reason);
  memcpy(buf, &data, sizeof(data));
  buf += sizeof(data);

  pdu_tableevent te;
  memset(&te, 0, sizeof(te));
  te.number = htons(table->getNumber());
  te.ipaddress = htonl(table->getAddr());
  te.port = htons(table->getPort());
  memcpy(buf, &te, sizeof(te));
  buf += sizeof(te);
  
  // XXX sendToLounges();
  return sendToLoadBalancer();
}

int CpduWaitinglistEvent::sendRemove(u_int16_t queuenumber,
                                     CTournament* t)
{
  return sendDelta(queuenumber, t, WLE_Tournament_Remove);
}

int CpduWaitinglistEvent::sendAdd(u_int16_t queuenumber,
                                  CTournament* t)
{
  return sendDelta(queuenumber, t, WLE_Tournament_Add);
}

int CpduWaitinglistEvent::sendDelta(u_int16_t    queuenumber,
                                    CTournament* tournament,
                                    u_int16_t    reason)
{
  length_ += sizeof(pdu_waitinglisteventheader) + sizeof(pdu_tournamentevent);
  stuffHeader();
  
  char* buf = value_ + PDU_HEADER_SIZE;
  
  pdu_waitinglisteventheader data;
  memset(&data, 0, sizeof(data));
  data.queueNumber = htons(queuenumber);
  data.reason = htons(reason);
  memcpy(buf, &data, sizeof(data));
  buf += sizeof(data);

  pdu_tournamentevent te;
  memset(&te, 0, sizeof(te));
  te.number = htons(tournament->getNumber());
  memcpy(buf, &te, sizeof(te));
  buf += sizeof(te);
  
  // XXX sendToLounges();
  return sendToLoadBalancer();
}


void CpduWaitinglistEvent::grok(SOCKET sd, CLoungeProxy* otherLounge)
{
  CLounge* lounge = CLounge::Inst();
  if (!lounge)
    return;

  int rc = 0;

  pdu_waitinglisteventheader* header = 
    reinterpret_cast<pdu_waitinglisteventheader*>(value_ + PDU_HEADERSIZE);
  u_int16_t queueNumber = ntohs(header->queueNumber);
  u_int16_t reason = ntohs(header->reason);
  header++;

  switch (reason)
  {
  // ok ok, the dreaded switch on type... but there
  // will NEVER be new cases to add here
  case WLE_Table_Add:
    {
      pdu_tableevent* ptr = reinterpret_cast<pdu_tableevent*>(header);
      lounge->addRemoteTable(queueNumber,
                             ntohs(ptr->number),
                             ntohl(ptr->ipaddress),
                             ntohs(ptr->port));
    }
    break;

  case WLE_Table_Remove:
    {
      pdu_tableevent* ptr = reinterpret_cast<pdu_tableevent*>(header);
      lounge->removeRemoteTable(queueNumber,
                                ntohs(ptr->number));
    }
    break;

  case WLE_Tournament_Add:
    {
      pdu_tournamentevent* ptr = reinterpret_cast<pdu_tournamentevent*>(header);
      //rc = lounge->addTournamentProxy(queueNumber,
      //                                ntohs(ptr->number));
    }
    break;

  case WLE_Tournament_Remove:
    {
      pdu_tournamentevent* ptr = reinterpret_cast<pdu_tournamentevent*>(header);
      //rc = lounge->removeTournamentProxy(queueNumber,
      //                                   ntohs(ptr->number));
    }
    break;

  default:
    break;
  }

  delete this;
}

void CpduWaitinglistEvent::grok(SOCKET sd, CLoadBalancer* lb)
{
  CLounge* lounge = CLounge::Inst();
  if (!lounge)
    return;

  int rc = 0;

  pdu_waitinglisteventheader* header = 
    reinterpret_cast<pdu_waitinglisteventheader*>(value_ + PDU_HEADERSIZE);
  u_int16_t queueNumber = ntohs(header->queueNumber);
  u_int16_t reason = ntohs(header->reason);
  header++;

  switch (reason)
  {
  // ok ok, the dreaded switch on type... but there
  // will NEVER be new cases to add here
  case WLE_Table_Add:
    {
      pdu_tableevent* ptr = reinterpret_cast<pdu_tableevent*>(header);
      //rc = lounge->addTableProxy(queueNumber,
      //                           ntohs(ptr->number),
      //                           ntohl(ptr->ipaddress),
      //                           ntohs(ptr->port),
    }
    break;

  case WLE_Table_Remove:
    {
      pdu_tableevent* ptr = reinterpret_cast<pdu_tableevent*>(header);
      //rc = lounge->removeTableProxy(queueNumber,
      //                              ntohs(ptr->number),
      //                              ntohl(ptr->ipaddress),
      //                              ntohs(ptr->port),
    }
    break;

  case WLE_Tournament_Add:
    {
      pdu_tournamentevent* ptr = reinterpret_cast<pdu_tournamentevent*>(header);
      //rc = lounge->addTournamentProxy(queueNumber,
      //                                ntohs(ptr->number));
    }
    break;

  case WLE_Tournament_Remove:
    {
      pdu_tournamentevent* ptr = reinterpret_cast<pdu_tournamentevent*>(header);
      //rc = lounge->removeTournamentProxy(queueNumber,
      //                                   ntohs(ptr->number));
    }
    break;

  default:
    break;
  }

  delete this;
}