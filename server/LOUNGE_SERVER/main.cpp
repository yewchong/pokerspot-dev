/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : main.cpp
  Created  : 04/16/2000

  OVERVIEW : Lounge server entry point.

 ****************************************************************************/

#include "server.h"
#include <time.h>


int main(int argc, char* argv[])
{
  srand((unsigned)time(NULL));

  int rc = -1;
  Server server;

  if (-1 != server.initialize(argc, argv))
  {
      rc = server.mainLoop();
  }

  server.shutdown();

  return rc;
}


