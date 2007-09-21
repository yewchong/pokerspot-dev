/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduRedirectLogin
  Created  : 09/30/2001

  OVERVIEW : This class is used by the Load Balancer to 
             redirect a player to the appropriate Lounge Server.

 ****************************************************************************/

#include "pdu/pduredirectlogin.h"
#include "loadbalancer/lounge.h"
#include "common/networkalign.h"


#pragma NETWORK_ALIGN_BEGIN
struct pdu_redirectlogin
{
  u_int32_t ipaddress;
  u_int16_t port;
};
#pragma NETWORK_ALIGN_END


CpduRedirectLogin::CpduRedirectLogin()
{
  type_ = PDU_LB_RedirectLogin;
  length_ = PDU_HEADER_SIZE + sizeof(pdu_redirectlogin);
  stuffHeader();
}

int CpduRedirectLogin::sendRedirectLogin(SOCKET sd,
                                         u_int32_t ipaddr,
                                         u_int16_t port)
{
  char* buf = value_ + PDU_HEADER_SIZE;

  pdu_redirectlogin data;
  ::ZeroMemory(&data, sizeof(data));

  data.ipaddress = htonl(ipaddr);
  data.port = htons(port);

  memcpy(buf, &data, sizeof(data));

  return sendTo(sd);
}
