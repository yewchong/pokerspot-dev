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

#include "loadbalancer/pduloungeinfo.h"
#include "lounge/lounge.h"


REGISTER_INCOMING_PDU(PDU_LB_LoungeInfo, CpduLoungeInfo);


CpduLoungeInfo::CpduLoungeInfo()
{}


void CpduLoungeInfo::grok(SOCKET sd, CLoadBalancer* lb)
{
  CLounge* thisLounge = CLounge::Inst();

  char* buf = value_ + PDU_HEADER_SIZE;

  // Save information about the other Lounge Servers,
  // establish connections if there are new ones

  pdu_loungeinfo loungeInfo;
  ::ZeroMemory(&loungeInfo, sizeof(loungeInfo));

  pdu_loungeinfo* ploungeInfo = reinterpret_cast<pdu_loungeinfo*>(buf);
  loungeInfo.numEntries = ntohs(ploungeInfo->numEntries);
  loungeInfo.operation = ntohs(ploungeInfo->operation);

  int extraBytes = (length_ - PDU_HEADER_SIZE - sizeof(pdu_loungeinfo));
  assert(loungeInfo.numEntries == (extraBytes / sizeof(pdu_loungeentry)));

  buf += sizeof(loungeInfo);
  pdu_loungeentry* ploungeEntry = reinterpret_cast<pdu_loungeentry*>(buf);

  for (int i = 0; i < loungeInfo.numEntries; i++)
  {
    pdu_loungeentry loungeEntry;
    
    loungeEntry.number = ntohs(ploungeEntry->number);
    loungeEntry.port = ntohs(ploungeEntry->port);
    loungeEntry.ipaddress = ntohl(ploungeEntry->ipaddress);

    if (loungeInfo.operation == CpduLoungeInfo::Add)
    {
      // Insert new remote Lounge Server
    }
    else if (loungeInfo.operation == CpduLoungeInfo::Remove)
    {
      // Remove remote Lounge Server
    }
    else if (loungeInfo.operation == CpduLoungeInfo::SetNumber)
    {
      // Set number for this Lounge
      thisLounge->setNumber(loungeEntry.number);
    }

    ploungeEntry++;
  }

  delete this;
}
