#ifndef __network_ssl_h__
#define __network_ssl_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : ssl.cpp
  Created  : 1/8/2000

  OVERVIEW : SSL interface functions.

 ****************************************************************************/

extern
BOOL PSI_SSL_Init();

extern
BOOL PSI_SSL_Start(SOCKET sd);

extern
int PSI_SSL_Read(SOCKET sd, char* buf, int nbytes);

extern
int PSI_SSL_Write(SOCKET sd, char* buf, int nbytes);

extern
int PSI_SSL_Close(SOCKET sd);

extern
void PSI_SSL_Cleanup();

extern
void PSI_SSL_Enable(BOOL);

extern
BOOL PSI_SSL_IsEnabled();


#endif