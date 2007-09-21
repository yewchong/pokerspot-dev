#ifndef __lb_pduplayerhandshake_h__
#define __lb_pduplayerhandshake_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduPlayerHandshake
  Created  : 09/25/2001

  OVERVIEW : This class reads the Load Balancer <-> Player handshake
             data from the network.
             
             NOTE: the socket is in blocking mode during the handshake.

 ****************************************************************************/

#include "pdu/pdu.h"
#include "common/networkalign.h"

#define PDU_CHECKSUMSIZE 16


#pragma NETWORK_ALIGN_BEGIN
struct pdu_version
{
  u_int16_t platform;
  u_byte_t  checksum[PDU_CHECKSUMSIZE];
};
#pragma NETWORK_ALIGN_END


class CpduPlayerHandshake : public CPDU
{
public:
  CpduPlayerHandshake();

  int readHandshake(SOCKET);

  u_int16_t       getPlatform() const;
  const u_byte_t* getChecksum() const;
  const char*     getUsername() const;

private:
  char sz_username_[PDU_STRINGSIZE+1];
};

inline u_int16_t CpduPlayerHandshake::getPlatform() const
{
	const pdu_version* pdu = 
    reinterpret_cast<const pdu_version*>(value_ + PDU_HEADERSIZE);
	return ntohs(pdu->platform);
}

inline const u_byte_t* CpduPlayerHandshake::getChecksum() const
{
	const pdu_version* pdu = 
    reinterpret_cast<const pdu_version*>(value_ + PDU_HEADERSIZE);
	return pdu->checksum;
}

inline const char* CpduPlayerHandshake::getUsername() const
{
	return sz_username_;
}



#endif
