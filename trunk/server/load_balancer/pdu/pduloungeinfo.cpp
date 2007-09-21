/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduLoungeInfo
  Created  : 09/27/2001

  OVERVIEW : This class is used by the Load Balancer to send

             - currently connected lounges a new lounge
             - new lounge to already connected lounges

             when a new Lounge Server logs in to the Load Balancer.

             This will effectively let every Lounge Server to
             know of every other Lounge Server, creating a fully
             connected mesh.

 ****************************************************************************/

#include "pdu/pduloungeinfo.h"
#include "loadbalancer/loadbalancer.h"
#include "common/errorlog.h"


CpduLoungeInfo::CpduLoungeInfo()
{
  type_ = PDU_LB_LoungeInfo;
  length_ = PDU_HEADER_SIZE;
  stuffHeader();
}


int CpduLoungeInfo::sendNewLoungeInfo(CLounge* newLounge)
{
  CLoadBalancer* lb = CLB::Inst();

  int rc = 0;

  char* buf = value_ + PDU_HEADER_SIZE;

  //
  // Tell new lounge its number
  //
  pdu_loungeinfo loungeInfo;
  pdu_loungeentry loungeEntry;

  loungeInfo.operation = htons((u_short)CpduLoungeInfo::SetNumber);
  loungeInfo.numEntries = htons(1);
  memcpy(buf, &loungeInfo, sizeof(loungeInfo));
  buf += sizeof(loungeInfo);

  loungeEntry.number = htons(newLounge->getNumber());
  loungeEntry.port = htons(newLounge->getPort());
  loungeEntry.ipaddress = htonl(newLounge->getIpAddress());

  memcpy(buf, &loungeEntry, sizeof(loungeEntry));
  buf += sizeof(loungeEntry);

  length_ = PDU_HEADER_SIZE + sizeof(loungeInfo) + sizeof(loungeEntry);
  stuffHeader();

  if (newLounge->getSocket() != INVALID_SOCKET)
    rc = sendTo(newLounge->getSocket());

  //
  // Inform new lounge about existing lounges
  //
  buf = value_ + PDU_HEADER_SIZE;
  int numLounges = lb->numLoungesLoggedIn(newLounge);

  loungeInfo.operation = htons((u_short)CpduLoungeInfo::Add);
  loungeInfo.numEntries = htons(numLounges);
  memcpy(buf, &loungeInfo, sizeof(loungeInfo));
  buf += sizeof(loungeInfo);

  length_ = PDU_HEADER_SIZE + sizeof(loungeInfo) + numLounges * sizeof(loungeEntry);
  stuffHeader();
  
  if (length_ >= MAXBUFFERSIZE)
  {
    Err::Log() << "CpduLoungeInfo: length_ > MAXBUFFERSIZE\n";
    return -1;
  }

  for (LoungeIterator li = CLB::Inst()->getLounges();
       li.more(); li.next())
  {
    CLounge* lounge = li.current();

    if (lounge->isLoggedIn())
    {
      loungeEntry.number = htons(lounge->getNumber());
      loungeEntry.port = htons(lounge->getPort());
      loungeEntry.ipaddress = htonl(lounge->getIpAddress());
      
      memcpy(buf, &loungeEntry, sizeof(loungeEntry));
      buf += sizeof(loungeEntry);
    }
  }

  if (newLounge->getSocket() != INVALID_SOCKET)
    rc = sendTo(newLounge->getSocket());

  //
  // Inform existing Lounges about new Lounge
  //
  buf = value_ + PDU_HEADER_SIZE;
  length_ = PDU_HEADER_SIZE + sizeof(loungeInfo) + 1 * sizeof(loungeEntry);
  stuffHeader();

  numLounges = 1;
  loungeInfo.operation = htons((u_short)CpduLoungeInfo::Add);
  loungeInfo.numEntries = htons(numLounges);
  memcpy(buf, &loungeInfo, sizeof(loungeInfo));
  buf += sizeof(loungeInfo);

  loungeEntry.number = htons(newLounge->getNumber());
  loungeEntry.port = htons(newLounge->getPort());
  loungeEntry.ipaddress = htonl(newLounge->getIpAddress());      

  memcpy(buf, &loungeEntry, sizeof(loungeEntry));

  for (li = CLB::Inst()->getLounges();
       li.more(); li.next())
  {
    CLounge* lounge = li.current();

    if (lounge->isLoggedIn())
    {
      rc = sendTo(lounge->getSocket());
    }
  } 
  
  return rc;
}

int CpduLoungeInfo::sendRemoveLoungeInfo(CLounge* removedLounge)
{
  int rc = 0;
  
  char* buf = value_ + PDU_HEADER_SIZE;

  pdu_loungeinfo loungeInfo;
  pdu_loungeentry loungeEntry;
  
  //
  // Inform other Lounges about the Lounge that is going away
  //
  buf = value_ + PDU_HEADER_SIZE;
  length_ = PDU_HEADER_SIZE + sizeof(loungeInfo) + 1 * sizeof(loungeEntry);
  stuffHeader();

  loungeInfo.operation = htons((u_short)CpduLoungeInfo::Remove);
  loungeInfo.numEntries = htons(1);
  memcpy(buf, &loungeInfo, sizeof(loungeInfo));
  buf += sizeof(loungeInfo);

  loungeEntry.number = htons(removedLounge->getNumber());
  loungeEntry.port = htons(removedLounge->getPort());
  loungeEntry.ipaddress = htonl(removedLounge->getIpAddress());

  memcpy(buf, &loungeEntry, sizeof(loungeEntry));

  rc = broadcast(removedLounge->getSocket());

  return rc;
}