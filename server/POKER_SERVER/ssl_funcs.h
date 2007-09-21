#ifndef __psi_ssl_funcs_h__
#define __psi_ssl_funcs_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : sslfuncs.h
  Created  : 04/24/2000

  OVERVIEW : Wrapper functions for the SSL versions of socket functions.

             <socket,SSL*> pairs are stored into a hash table, lookup
             is constant time.

 ****************************************************************************/

extern int init_ssl();
extern int ssl_accept(int sockfd);
extern int ssl_connect(int sockfd);
extern int ssl_peek(int sockfd, char* buffer, int nbytes);
extern int ssl_pending(int sockfd);
extern int ssl_read(int sockfd, char* buffer, int nbytes);
extern int ssl_write(int sockfd, const char* buffer, int nbytes);
extern int ssl_close(int sockfd);
extern int ssl_shutdown();
extern bool ssl_checksocket(int sockfd);

#endif