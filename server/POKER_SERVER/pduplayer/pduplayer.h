#ifndef __table_pduplayer_h__
#define __table_pduplayer_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduPlayer
  Created  : 1/1/2001

  OVERVIEW : This class is used to send player state to clients.

 ****************************************************************************/


#include "pduhandler/pduhandler.h"
#include "common/chips.h"

class CTable;
class CPlayer;

#define PLAYER_LOGOUT     2
#define PLAYER_SITIN      4
#define PLAYER_SITOUT     8
#define PLAYER_CHIPUPDATE 32
// client reserves states 64, 128
#define PLAYER_ZOMBIE     256

#pragma NETWORK_ALIGN_BEGIN
struct player_pdu
{
  u_int16_t numplayers;
  u_int16_t slot;
  u_int16_t state;
  CChips::chip_amount_t chips;
};
#pragma NETWORK_ALIGN_END

class CpduPlayer : public Cpdu
{
public:
	CpduPlayer(CTable*);

  static int SendPlayerPdu(CPlayer*, u_int16_t reason);

  int sendPlayerPdu(CPlayer*, u_int16_t reason);
  int sendPlayerRaw(CPlayer*, u_int16_t reason);
  int sendChipUpdate(CPlayer* newPlayer, CPlayer* oldPlayer);

private:
	CTable* table_;
};

#endif