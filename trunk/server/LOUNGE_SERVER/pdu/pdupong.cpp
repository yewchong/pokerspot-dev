#include "pdupong.h"
#include "pduping.h"
#include "pingcounter.h"
#include "lounge/player.h"


REGISTER_INCOMING_PDU(PDU_Pong, CpduPong);

CpduPong::CpduPong()
{}

void CpduPong::grok(SOCKET sd, CPlayer* player)
{
  if (player)
  {
    pdu_ping* ping = (pdu_ping*)(value_ + PDU_HEADER_SIZE);
    u_int32_t sent = ntohl(ping->data);
    
    long now = time(NULL);
    player->getPingCounter()->setPongReceived(now);
  }
  
  // this pdu is done
  delete this;
}
