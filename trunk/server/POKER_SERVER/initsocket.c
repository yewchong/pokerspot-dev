#include <winsock.h>
#include <stdio.h>

int InitWinSock(void)
{
    WSADATA wsaData;
    WORD wVersionRequested;
    int err;

    /* Ask for Winsock 1.1 functionality */
    wVersionRequested = MAKEWORD( 1, 1 );
    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 ) {
      printf("Error %d: Winsock not available\n", err);
      return 1;
    }
    return 0;
}


int ShutdownWinSock()
{
  WSACleanup();
  return 0;
}