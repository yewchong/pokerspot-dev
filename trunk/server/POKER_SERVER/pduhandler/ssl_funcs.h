#ifndef __table_ssl_funcs_h__
#define __table_ssl_funcs_h__


#define _pdu_ssl_funcs_h
#include "includes.h"
#include "pdu.h"
#include "global.h"  


extern int Ppeek(int connfd);
extern int Pwrite(int connfd, char*buffer, int length);
extern int Pread(int connfd, char* buffer, int length);
extern int Paccept(int connfd, SSL_CTX* ctx);
extern int Pclosesocket(int connfd);


#endif
