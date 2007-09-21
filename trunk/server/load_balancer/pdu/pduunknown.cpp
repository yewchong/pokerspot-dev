/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : CpduUnknown
  Created  : 04/16/2000

  OVERVIEW : PDU class that handles incoming unknown pdus.

 ****************************************************************************/

#include "pdu/pduunknown.h"
#include "common/sys.h"
#include "common/errorlog.h"

REGISTER_INCOMING_PDU(PDU_Unknown, CpduUnknown);

//
// This type class is used to handle unrecognized incoming pdus.
// The 'grok' member function just prints out an error message.
//
// An unknown PDU type means either that the client version is
// out of synch (i.e., new client with new pdu types), or the
// version is hacked and is sending garbage. Either way, we should
// probably just close connection immediately. For now, the fact
// is only logged.
//

CpduUnknown::CpduUnknown()
{}


void CpduUnknown::grok(SOCKET)
{
  Err::Log() << "Ignoring unknown pdu type: " 
             << type_ << " length: " << length_ << CLog::nl;

  delete this;
}