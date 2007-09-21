/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : sslfuncs.h
  Created  : 04/24/2000

  OVERVIEW : SSL functions for storing and retrieving socket -> SSL
             associations.

             <socket,SSL*> pairs are stored into a hash table, lookup
             is constant time.

 ****************************************************************************/

#include "def.h"
#include "includes.h"
#include "sys.h"
#include "sync/sync.h"
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/err.h> 
#include <hash_map>
USING_NS_T(std, hash_map);

#define CERTF "server.pem"
#define KEYF  "server.pem"

// Link in the SLL libraries
#ifdef _WIN32
  #pragma comment(lib, "..\\..\\lib\\openssl\\out32dll\\ssleay32.lib")
  #pragma comment(lib, "..\\..\\lib\\openssl\\out32dll\\libeay32.lib")
#endif

static SSL_CTX* g_ssl_ctx = NULL;

int init_ssl()
{
    SSL_load_error_strings();
    SSLeay_add_ssl_algorithms();
    g_ssl_ctx = SSL_CTX_new(SSLv23_server_method());

    if (!g_ssl_ctx)
    {
        ERR_print_errors_fp(stderr);
        return -1;
    }

    if (SSL_CTX_use_certificate_file(g_ssl_ctx, CERTF, SSL_FILETYPE_PEM) < 0)
    {
        printf("SSL cert load failed\n");
        SSL_CTX_free(g_ssl_ctx);
        g_ssl_ctx = NULL;
        return -1;
    }

    if (SSL_CTX_use_PrivateKey_file(g_ssl_ctx, KEYF, SSL_FILETYPE_PEM) < 0)
    {
        printf("SSL rsa key load failed\n");
        SSL_CTX_free(g_ssl_ctx);
        g_ssl_ctx = NULL;
        return -1;
    }

    return 0;
}

//
// Global hash table for maintaining socket -> SSL associations
// 
typedef hash_map<int, SSL*> Socket2SSL; 
static Socket2SSL g_socket2SSL;
Mutex g_AssocMutex;

// Return the SSL associated with 'sockfd'.
SSL* lookup_ssl(int sockfd)
{
    SingleLock l(&g_AssocMutex);
    l.lock();

    // Do hash table lookup
    Socket2SSL::iterator pos = g_socket2SSL.find(sockfd);

    // Return associated SSL object if found
    if (pos != g_socket2SSL.end())
        return (*pos).second;

    return NULL;
}

// Return true if sockfd -> ssl association was added,
// return false if association was already stored.
bool add_socket_ssl_assoc(int sockfd, SSL* ssl)
{
    SingleLock l(&g_AssocMutex);
    l.lock();

    return (g_socket2SSL.insert(Socket2SSL::value_type(sockfd, ssl)).second);
}


// Accept new connection - stores socket -> SSL association
int ssl_accept(int sockfd)
{
    if (!g_ssl_ctx)
    {
        printf("ssl_accept: SSL not initialized\n");
        return -1;
    }

    SSL* ssl = SSL_new(g_ssl_ctx);
    if (ssl == NULL)
    {
        Sys_LogError("Out of memory (SSL_new)");
        return -1;
    }

    SSL_set_fd(ssl, sockfd);
    int err = SSL_accept(ssl);

    if (err == -1)
    {
        int e = SSL_get_error(ssl, -1);
        Sys_LogError("SSL_accept fails");
        Sys_LogError(ERR_error_string(e, NULL));
        SSL_free(ssl);
        return -1;
    }

    printf("SSL connection using %s\n", SSL_get_cipher(ssl));

    if (add_socket_ssl_assoc(sockfd, ssl))
    {
        return 0;
    }
    else
    {
        SSL_free(ssl);
        return -1;
    }
}


int ssl_peek(int sockfd, char* buffer, int nbytes)
{
    SSL* ssl = lookup_ssl(sockfd);
    if (ssl)
        return SSL_peek(ssl, buffer, nbytes);
    else
    {   
        printf("No SSL for socket %d\n", sockfd);
        return -1;
    }
}


int ssl_check_nbio(int sockfd, int err)
{
    SSL* ssl = lookup_ssl(sockfd);
    if (ssl)
    {
        int code = SSL_get_error(ssl, err);

        static int c = 10;
        if (code != 6 && c > 0)
        {
            c--;
            char buf[100];
#ifdef _WIN32
            sprintf(buf, "ssl_check_nbio: %i, %i, %i\n", err, code, WSAGetLastError());
#else
            sprintf(buf, "ssl_check_nbio: %i, %i, %i\n", err, code, errno);
#endif
            Sys_LogError(buf);
        }
    }
    else
    {
        static int c = 10;
        if (c > 0)
        {
            c--;
            char buf[100];
            sprintf(buf, "ssl_check_nbio: No SSL for socket %d\n", sockfd);
            Sys_LogError(buf);
        }
    }
    return err;
}



int ssl_read(int sockfd, char* buffer, int nbytes)
{
    SSL* ssl = lookup_ssl(sockfd);
    if (ssl)
        return SSL_read(ssl, buffer, nbytes);
    else
    {   
        printf("No SSL for socket %d\n", sockfd);
        return -1;
    }
}


int ssl_write(int sockfd, const char* buffer, int nbytes)
{
    SSL* ssl = lookup_ssl(sockfd);
    if (ssl)
    {
        int c = SSL_write(ssl, buffer, nbytes);
        if (c != nbytes)
        {
            char buf[100];
            sprintf(buf, "ssl_write: %i %i (sd: %i)", c, nbytes, sockfd);
            Sys_LogError(buf);
        }
        return c;
    }
    else
    {   
        printf("No SSL for socket %d\n", sockfd);
        return -1;
    }
}

int ssl_close(int sockfd)
{
    SingleLock l(&g_AssocMutex);
    l.lock();

    close(sockfd);

    SSL* ssl = lookup_ssl(sockfd);
    if (ssl)
    {
        SSL_free(ssl);
        g_socket2SSL.erase(sockfd);
    }

    return 0;
}

// This is the client SSL context used for
// connection to the Load Balancer
static SSL_CTX* g_ssl_ctx_loadbalancer = NULL;


int ssl_connect(int sd)
{
  // Create separate SSL context for LB connection.
  // This is because in the lounge - LB connection the
  // lounge is the *client* and the SSL context has to be
  // created with the *client* method.
  if (g_ssl_ctx_loadbalancer == NULL)
    g_ssl_ctx_loadbalancer = SSL_CTX_new(SSLv2_client_method());

  if (g_ssl_ctx_loadbalancer == NULL)
    return -1; 

  SSL* ssl = SSL_new(g_ssl_ctx_loadbalancer);
  SSL_set_fd(ssl, sd);

  int err = SSL_connect(ssl);
  if (err == -1)
  {
      Sys_LogError("SSL_connect to Load Balancer fails");
      //ERR_print_errors_fp(stderr);
      SSL_free(ssl);
      return -1;
  }

  if (add_socket_ssl_assoc(sd, ssl))
      return 0;
  else
  {
      char s[100];
      sprintf(s, "Socket %d has already SSL association", sd);
      Sys_LogError(s);
      SSL_free(ssl);
      return -1;
  }

  return 0;
}

int ssl_shutdown()
{
    if (g_ssl_ctx)
    {
        SSL_CTX_free(g_ssl_ctx);
        g_ssl_ctx = NULL;
    }

    if (g_ssl_ctx_loadbalancer)
    {
        SSL_CTX_free(g_ssl_ctx_loadbalancer);
        g_ssl_ctx_loadbalancer = NULL;
    }

    return 0;
}



