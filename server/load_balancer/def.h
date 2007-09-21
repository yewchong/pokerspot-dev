#ifndef __lb_def_h__
#define __lb_def_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : def.h
  Created  : 09/21/2000

  OVERVIEW : CServer class contains the main loop of the Load Balancer.

 ****************************************************************************/


// This is the port the Load Balancer server will
// be listening for incoming lounge connections.
// Load Balancer will listen to incoming player
// connections at CWG_LOADBALANCER_PORT+1
#define CWG_LOADBALANCER_PORT  8300
#define CWG_DEFAULT_MAXTHREADS 50


// Timeout for SSL handshakes. If the handshake is not
// completed in this time the connection is aborted.
#define CWG_HANDSHAKE_TIMEOUT 15000


// XXX Read waiting list configuration from database!
#define CWG_NUM_WAITINGLISTS 36


#endif