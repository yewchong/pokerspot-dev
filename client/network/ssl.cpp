/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : ssl.cpp
  Created  : 1/8/2000

  OVERVIEW : SSL interface functions.

 ****************************************************************************/

#include "stdafx.h"
#include <winsock.h>
#include "network/errorlog.h"

#define USE_SSL_CRYPTO_
#ifdef USE_SSL_CRYPTO_

#include "openssl/ssl.h"
#include "openssl/err.h"
#include "network/network.h"
#ifdef PSPOT_TABLE_MODULE_
  #include "ui/registry.h"
#elif defined(PSPOT_SERVER_MODULE_)
  #include "registry.h"
#else
  #include "loungedlg/registry.h"
#endif
#include <assert.h>

// Link in the SLL libraries

#ifdef PSPOT_TABLE_MODULE_
#pragma comment(lib, "..\\lib\\openssl\\out32dll\\ssleay32.lib")
#pragma comment(lib, "..\\lib\\openssl\\out32dll\\libeay32.lib")
#else
#pragma comment(lib, "..\\..\\lib\\openssl\\out32dll\\ssleay32.lib")
#pragma comment(lib, "..\\..\\lib\\openssl\\out32dll\\libeay32.lib")
#endif

BEGIN_NAMESPACE0()
  SSL_CTX*    g_pCtx = 0;

  //SSL* g_pSSL = 0;

  #define MAX_CONNECTIONS 2
  struct Socket2SSL
  {
    SOCKET sd;
    SSL*   pSSL;
  } g_Socket2SSL[MAX_CONNECTIONS];

  SSL* GetSSL(SOCKET sd)
  {
    for (int i = 0; i < MAX_CONNECTIONS; i++)
    {
      if (g_Socket2SSL[i].sd == sd)
        return g_Socket2SSL[i].pSSL;
    }
    return NULL;
  }

  void SetSSL(SOCKET sd, SSL* pSSL)
  {
    for (int i = 0; i < MAX_CONNECTIONS; i++)
    {
      if (g_Socket2SSL[i].sd == INVALID_SOCKET)
      {
        g_Socket2SSL[i].sd = sd;
        g_Socket2SSL[i].pSSL = pSSL;
        return;
      }
      ASSERT(FALSE);
    }
  }

  void ClearSSL(SOCKET sd)
  {
    for (int i = 0; i < MAX_CONNECTIONS; i++)
    {
      if (g_Socket2SSL[i].sd == sd)
      {
        g_Socket2SSL[i].sd = INVALID_SOCKET;
        g_Socket2SSL[i].pSSL = NULL;
        return;
      }
      ASSERT(FALSE);
    }
  }

  void InitSocket2SSL()
  {
    for (int i = 0; i < MAX_CONNECTIONS; i++)
    {
      g_Socket2SSL[i].sd = INVALID_SOCKET;
      g_Socket2SSL[i].pSSL = NULL;
    }
  }

  SSL_METHOD* g_pMeth = 0;
  BOOL        g_bEnableSSL = TRUE;
END_NAMESPACE0()


#define CHK_NULL(x) if ((x)==NULL) return FALSE;

void PSI_SSL_Enable(BOOL enable)
{
  g_bEnableSSL = enable;
}


BOOL PSI_SSL_IsEnabled()
{
  return g_bEnableSSL;
}


BOOL PSI_SSL_Init()
{
  g_bEnableSSL = Registry::GetUseSSL(g_bEnableSSL);

  if (!g_bEnableSSL)
    return TRUE;

  SSLeay_add_ssl_algorithms();
  g_pMeth = SSLv2_client_method();
  SSL_load_error_strings();
  g_pCtx = SSL_CTX_new(g_pMeth);

  if (!g_pCtx)
  {
    AfxMessageBox("Can't initialize SSL");
    return FALSE;
  }

  return TRUE;
}


BOOL PSI_SSL_Start(SOCKET sd)
{
  InitSocket2SSL();

  if (!g_bEnableSSL)
    return TRUE;

  if (g_pCtx == NULL)
    return FALSE; // SSL_Init not called!

  SSL* pSSL = SSL_new(g_pCtx);
  CHK_NULL(pSSL);

  SSL_set_fd(pSSL, sd);
  SetSSL(sd, pSSL);

  int err = SSL_connect(pSSL);
  if (err == -1)
  {
    int sslerr = SSL_get_error(pSSL, err);
    int syserr = WSAGetLastError();
    CString s;
    s.Format("SSL_connect: sys: %i ssl: %i",
             syserr, sslerr);
    Network::LogError(s);

    s.Format("An error occurred while attempting to negotiate secure connection with the server:\n\n %s (%i %i)\n\nPlease retry logging in.",
             ERR_error_string(sslerr, NULL), sslerr, syserr);
    AfxMessageBox(s);
    return FALSE;
  }

  return TRUE;
}


int PSI_SSL_Read(SOCKET sd, char* buf, int nbytes)
{
  if (sd == INVALID_SOCKET) return 0;

  if (!g_bEnableSSL)
  {
    return recv(sd, buf, nbytes, 0);
  }

  SSL* pSSL = GetSSL(sd);
  ASSERT(pSSL);
  int rc = SSL_read(pSSL, buf, nbytes);

  if (rc == -1)
  {
    static int c = 20;
    if (c > 0)
    {
      c--;
      int err = SSL_get_error(pSSL, rc);
      CString s;
      s.Format("Read socket error: SSL: %s (%i) socket: %d",
               ERR_error_string(err, NULL), err, WSAGetLastError());
      Network::LogError(s);
    }
  }

  return rc;
}


int PSI_SSL_Write(SOCKET sd, char* buf, int nbytes)
{
  if (!g_bEnableSSL)
  {
    return send(sd, buf, nbytes, 0);
  }

  SSL* pSSL = GetSSL(sd);
  ASSERT(pSSL);
  int rc = SSL_write(pSSL, buf, nbytes);

  if (rc != nbytes)
  {
    int sslerr = SSL_get_error(pSSL, rc);
    int syserr = WSAGetLastError();

    CString s;
    s.Format("Write error: sys: %i ssl: %i",
             syserr, sslerr);
    Network::LogError(s);
  }

  return rc;
}


int PSI_SSL_Close(SOCKET sd)
{
  if (!g_bEnableSSL)
  {
    return 1;
  }

  SSL* pSSL = GetSSL(sd);
  ASSERT(pSSL);
  int rc = SSL_shutdown(pSSL);

  ClearSSL(sd);
  SSL_free(pSSL);

  return rc;
}


void PSI_SSL_Cleanup()
{
/*
  if (g_pSSL)
    SSL_free(g_pSSL);
  g_pSSL = 0;
*/

  if (g_pCtx)
    SSL_CTX_free(g_pCtx);
  g_pCtx = 0;
}


#else // Dummy versions that don't use SSL


void PSI_SSL_Enable(BOOL enable)
{
  ASSERT(enable == FALSE);
}

BOOL PSI_SSL_IsEnabled()
{
  return FALSE;
}


BOOL PSI_SSL_Init()
{
  return TRUE;
}


BOOL PSI_SSL_Start(SOCKET sd)
{
  return TRUE;
}


int PSI_SSL_Read(SOCKET sd, char* buf, int nbytes)
{
  return recv(sd, buf, nbytes, 0);
}


int PSI_SSL_Write(SOCKET sd, char* buf, int nbytes)
{
  return send(sd, buf, nbytes, 0);
}


int PSI_SSL_Close(SOCKET sd)
{
  return 0;
}


void PSI_SSL_Cleanup()
{
}

#endif 