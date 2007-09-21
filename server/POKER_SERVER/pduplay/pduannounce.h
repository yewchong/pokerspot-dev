#ifndef __table_pduannounce_h__
#define __table_pduannounce_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduAnnounce
  Created  : 08/13/2000

  OVERVIEW : This pdu is used to announce a winner of a hand.

 ****************************************************************************/


#include "pduhandler/pduhandler.h"

#define MAXANNOUNCE     256
#define ANNOUNCE_FLAG_MESSAGE 0xfffe // used in 7-stud

class CTable;
class CPlayer;
class CChips;

class CpduAnnounce : public Cpdu
{
public:
  CpduAnnounce(CTable*);
  int sendAnnounce(CPlayer*, const CChips&);
  int sendAnnounce(CPlayer*, const CChips&, const char*, const char*);
  int sendAnnounce(const char* msg);
  int sendAnnounce(const char* msg, int flags); // 7-stud

private:
  CTable* table_;
};


#endif  
