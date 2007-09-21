/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : main.cpp
  Created  : 09/21/2000

  OVERVIEW : Load Balancer server entry point

 ****************************************************************************/

#include "server.h"
#include "common/sys.h"
#include "common/errorlog.h"
#include <time.h>


int main(int argc, char** argv)
{
  // Create the single CErrorLog instance
  CErrorLog log("errorlog-lb.txt");
  Err::Log() << "Wagerware Load Balance Server started at "
             << Sys_GetDate() << "\n\n";

  srand((unsigned)time(NULL));

  int rc = -1;
  CServer server;

  if (server.initialize(argc, argv) != -1)
  {
    rc = server.mainLoop();
  }

  server.shutdown();

  Err::Log() << "Wagerware Load Balance Server shut down at "
             << Sys_GetDate() << "\n\n";
  
  return rc;
}
