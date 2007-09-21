/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduWaitinglistConfig
  Created  : 10/06/2001

  OVERVIEW : This class is used by the Load Balancer to send
             information about waiting lists to Lounge Servers.

 ****************************************************************************/


#include "pdu/pduwaitinglistconfig.h"
#include "loadbalancer/loadbalancer.h"
#include "def.h"
#include "common/errorlog.h"


CpduWaitinglistConfig::CpduWaitinglistConfig()
{
  type_ = PDU_LB_WaitinglistConfig;
  length_ = PDU_HEADER_SIZE; // will change
  stuffHeader();
}

//
// For now, this mfunction assigns waiting list
// ownership evenly to existing lounges. I.e.,
// if there are 3 lounge servers and 30 waiting
// lists, then each lounge server will own 10
// waiting list.
//
// TODO: read the ownership configuration from
// the database.
//
int CpduWaitinglistConfig::sendWaitinglistOwners()
{
  int rc = 0;

  char* buf = value_ + PDU_HEADER_SIZE;

  int numWaitinglists = CWG_NUM_WAITINGLISTS;
  int numLounges = CLB::Inst()->numLounges();
  if (numLounges == 0) return 0;

  length_ = PDU_HEADERSIZE + 
              sizeof(pdu_waitinglistheader) + 
              numWaitinglists * sizeof(pdu_waitinglistowner);
  stuffHeader();

  if (length_ >= MAXBUFFERSIZE)
  {
    Err::Log() << "CpduWaitinglistConfig: length_ > MAXBUFFERSIZE\n";
    return -1;
  }


  pdu_waitinglistheader header;
  ::ZeroMemory(&header, sizeof(header));

  header.numEntries = htons(numWaitinglists);
  header.operation = htons(CpduWaitinglistConfig::SetOwner);

  memcpy(buf, &header, sizeof(header));
  buf += sizeof(header);

  pdu_waitinglistowner data;
  ::ZeroMemory(&data, sizeof(data));

  // Distribute waiting lists evenly to lounges
  int owner = 1;
  float curr = 1.0f,
        incr = float(numLounges) / float(numWaitinglists); 

  for (int i = 0; i < numWaitinglists; i++)
  {
    // Linear interpolation to distribute waiting list
    // ownership evenly to lounges.
    owner = (int)curr;
    curr += incr;

    data.number = htons(i);
    data.owner = htons(owner);

    cout << "Waiting list " << i
         << " owner lounge " << owner << endl;

    memcpy(buf, &data, sizeof(data));
    buf += sizeof(data);

  }

  return broadcast();
}