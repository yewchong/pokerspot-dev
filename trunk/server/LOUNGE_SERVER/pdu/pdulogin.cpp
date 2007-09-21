/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduLogin
  Created  : 04/17/2000

  OVERVIEW : Login PDU.

 ****************************************************************************/

#include "pdu/pdulogin.h"
#include "lounge/lounge.h"
#include "sys.h"

REGISTER_INCOMING_PDU(PDU_Lounge_Login, CpduLogin);


CpduLogin::CpduLogin()
{
  memset(sz_username, 0, sizeof(sz_username));
  memset(sz_password, 0, sizeof(sz_password));
}

void CpduLogin::grok(SOCKET sd, CPlayer*)
{
  // Copy username & password to zero-terminated
  // strings
	pdu_login* data = (pdu_login*)(value_ + PDU_HEADERSIZE);
  memcpy(sz_username, data->username, PDU_STRINGSIZE);
  sz_username[PDU_STRINGSIZE] = '\0';
  memcpy(sz_password, data->password, PDU_STRINGSIZE);
  sz_password[PDU_STRINGSIZE] = '\0';

  CLounge::Inst()->login(sd,
                         getUsername(),
                         getPassword(),
                         getPlatform(),
                         getChecksum());

  // this pdu is done
  delete this;
}
