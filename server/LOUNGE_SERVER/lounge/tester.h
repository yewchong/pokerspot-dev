#ifndef __psi_tester_h__
#define __psi_tester_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CTester
  Created  : 01/01/2001

  OVERVIEW : This class adds bots to the waiting lists.

 ****************************************************************************/



class CTester
{
public:
  CTester();
  virtual void tick(long now);

private:
    int numPlayers_;
    int counter_;
    int state_;
    long lastTest_, lastTick_;
};

#endif
