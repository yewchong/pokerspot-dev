/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CTester
  Created  : 01/01/2001

  OVERVIEW : This class adds bots to the waiting lists.

 ****************************************************************************/

#include "lounge/tester.h"
#include "lounge/lounge.h"
#include "tournament/basetournamentqueue.h"
#include "dbhandler/dbinterface.h"
#include <minmax.h>


CTester::CTester()
  :
  numPlayers_(0),
  counter_   (0),
  state_     (0),
  lastTest_  (0),
  lastTick_  (0)
{}


//
// Keep adding players to tournament queues ad infinitum
//
void CTester::tick(long now)
{
  if (now - lastTick_ < 5)
    return;
  lastTick_ = now;

  if (state_ == -1 && numPlayers_ > 0)
  {
    // Create test players
    if (!CdbInterface::Inst()->createTestPlayers(numPlayers_))
      state_ = -2;
    
    state_ = 0;
  }
  
  if (state_ == 0)
  {
    long maxPlayers = 0;
    CdbInterface::Inst()->getMaxPlayers(maxPlayers);

    numPlayers_ = min(numPlayers_, maxPlayers);

    // Logging in players
    if (counter_ < numPlayers_)
    {
      // Log in 10 players at a time
      for (int i = 0; (i < 10) && (counter_ < numPlayers_); i++)
      {
        char buf[17];      
        sprintf(buf, "bot_%i", (counter_ + 1));
      
        struct in_addr inaddr;
        memset(&inaddr, 0, sizeof(inaddr));
      
        string pw(buf);
        tolower(pw);
      
        CLounge::Inst()->addPlayer(-1, &inaddr);
        CLounge::Inst()->login(-1, buf, pw.c_str(), 0, NULL);
        counter_++;
      }
    }
    else
    {
      counter_ = 0;
      state_ = 1;
    }
  }
  
  if (state_ == 1)
  {
    if (now - lastTest_ > 5)
    {
      if (counter_ < numPlayers_)
      {
        // Add players to tournament queue
        char buf[17];
        sprintf(buf, "bot_%i", (counter_ + 1));
        
        CPlayer* p = CLounge::Inst()->findPlayer(buf);
        if (p)
        {
          // Put the player to a random tournament queue
          int index = (rand() % 5) + 1;
          
          CBaseTournamentQueue* tq =
            CBaseTournamentQueue::FindQueueByIndex(index);
          if (tq)
          {
            CLounge::Inst()->queuePlayer(p, tq->getQueueNumber());
          }
        }
        counter_++;
      }
      else
      {       
        state_ = 1; // Start over from first bot again
        counter_ = 0;
      }   
    }
  }

  const int TestInterval = 10;
  if (now - lastTest_ > TestInterval)
  {
    int oldNum = numPlayers_;
    CdbInterface::Inst()->getTestState(numPlayers_);
    lastTest_ = now;

    if (oldNum != numPlayers_)
    {
      state_ = 0;
      counter_ = 0; // log in new players
    }
  }
}
