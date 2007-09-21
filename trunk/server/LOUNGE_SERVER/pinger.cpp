#include "pinger.h"
#include "pingcounter.h"
#include "lounge/player.h"
#include "lounge/lounge.h"
#include "pdu/pduping.h"
#include "pdu/pdunotify.h"
#include "sys.h"


void CPinger::tick(long now)
{
  // Do the ping iteration only every fifth tick
  static int i = 0;
  if (++i < 5)
    return;
  i = 0;
  
  for (PlayerIterator p = CLounge::Inst()->allPlayers();
       p.more(); p.next())
  {
    CPlayer* player = p.current();
    if (player->getNetworkError())
    {
      // Player's connection has died
      char s[200];
      sprintf(s, "Network error (3), player %s.\n", player->getUsername());
      Sys_LogError(s);
      
      CpduNotify pdu;
      pdu.sendNotifyMessage(player->getSocket(), "Timed out, closing connection.");
      
      CLounge::Inst()->logout(player->getSocket(), player, true);
    }
    else
    {
      if (player->getPingCounter())
        tickPlayer(player->getPingCounter(), now);
    }
  }
  
}

void CPinger::tickPlayer(CPingCounter* pingCounter, long now)
{
    // Send a ping after interval
    if (now - pingCounter->getLastPingSent() > PING_DELTA)
    {
        CpduPing pdu;
        pdu.sendPing(pingCounter->getPlayer(), now);
        pingCounter->setLastPingSent(now);
    }

    pingCounter->tickPing(now);
    
    if (pingCounter->pingsFailed() > MAX_PING_FAILURES)
    {
        // Don't mark bots as having a network error
        string test_str = pingCounter->getPlayer()->getUsername();
        if (test_str.find("bot_") == 0)
            ;
        else
            pingCounter->getPlayer()->setNetworkError(true);
    }
}

