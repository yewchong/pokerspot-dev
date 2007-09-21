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
#include <map>
USING_NS_T(std, map);

#define CERTF "server.pem"
#define KEYF  "server.pem"

// Link in the SLL libraries
#ifdef _WIN32
  #pragma comment(lib, "..\\..\\lib\\openssl\\out32dll\\ssleay32.lib")
  #pragma comment(lib, "..\\..\\lib\\openssl\\out32dll\\libeay32.lib")
#endif

int ssl_check_nbio(int sockfd, int err);

// SSL Thread protection

static HANDLE lock_cs[CRYPTO_NUM_LOCKS];
void thread_locking_callback(int mode, int type, const char* file, int line);
unsigned long thread_id();

void thread_setup()
{
  int i;
  
  for (i=0; i<CRYPTO_NUM_LOCKS; i++)
  {
    lock_cs[i] = Sys_CreateMutex();
  }
  
  CRYPTO_set_id_callback((unsigned long (*)())thread_id);
  CRYPTO_set_locking_callback(thread_locking_callback);
}

void thread_cleanup()
{
  int i;
  
  CRYPTO_set_locking_callback(NULL);
  for (i=0; i<CRYPTO_NUM_LOCKS; i++)
  {
    Sys_DestroyMutex(lock_cs[i]);
    lock_cs[i] = NULL;
  }
}

void thread_locking_callback(int mode, int type, const char* file, int line)
{
  if (mode & CRYPTO_LOCK)
	{
    Sys_LockMutex(lock_cs[type], INFINITE);
	}
  else
	{
    Sys_ReleaseMutex(lock_cs[type]);
	}
}

unsigned long thread_id()
{
  unsigned long ret = (unsigned long)GetCurrentThreadId();
  return ret;
}


// This is the server SSL context used for
// player connections.
static SSL_CTX* g_ssl_ctx = NULL;

int init_ssl()
{
    SSL_load_error_strings();
    SSLeay_add_ssl_algorithms();
    g_ssl_ctx = SSL_CTX_new(SSLv23_server_method());

    if (!g_ssl_ctx)
    {
        //ERR_print_errors_fp(stderr);
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

    thread_setup();

    return 0;
}

//
// Global map for maintaining socket -> SSL associations
// 
typedef map<int, SSL*> Socket2SSL; 
static Socket2SSL g_socket2SSL;

// Return the SSL associated with 'sockfd'.
SSL* lookup_ssl(int sockfd)
{
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
    return (g_socket2SSL.insert(Socket2SSL::value_type(sockfd, ssl)).second);
}

// PROTECT ALL SSL ACCESS WITH A MUTEX
// This takes also care of potential race
// conditions with the hash table being
// accessed from the two threads.

Mutex g_ssl_mutex;

int g_in_accept1 = 0,
    g_in_accept2 = 0;

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
        //ERR_print_errors();
        return -1;
    }

    g_in_accept1++;
    SSL_set_fd(ssl, sockfd);
    int err = SSL_accept(ssl);
    g_in_accept1--;

    if (err == -1 || err == 0)
    {
        int syserr = Sys_GetLastError();;
        int sslerr = SSL_get_error(ssl, err);
        char buf[100];
        sprintf(buf, "SSL_accept fails: %i %i %i",
                err, syserr, sslerr);
        Sys_LogError(buf);
        SSL_free(ssl);
        return -1;
    }

    printf("Accepted SSL connection using %s\n", SSL_get_cipher(ssl));

    g_in_accept2++;
    SingleLock l(&g_ssl_mutex);
    if (!l.lock())
        return -1;

    if (add_socket_ssl_assoc(sockfd, ssl))
    {
        g_in_accept2--;
        return 0;
    }
    else
    {
        g_in_accept2--;
        SSL_free(ssl);
        return -1;
    }
}


int ssl_peek(int sockfd, char* buffer, int nbytes)
{
    SingleLock l(&g_ssl_mutex);
    if (!l.lock())
        return -1;

    int err = 0;

    SSL* ssl = lookup_ssl(sockfd);
    if (ssl)
        err = SSL_peek(ssl, buffer, nbytes);
    else
    {   
        char buf[100];
        sprintf(buf, "No SSL for socket %d\n", sockfd);
        Sys_LogError(buf);
        return -1;
    }

    if (err < 0 || (err != nbytes))
    {
        static unsigned int i = 0;
        if (i++ < 30)
        {
            int syserr = Sys_GetLastError();
            int code = SSL_get_error(ssl, err);
            int retry = BIO_should_retry(SSL_get_rbio(ssl));
            char buf[1024];
            sprintf(buf, "SSL_peek: sock: %i, nbytes: %i, err: %i, sslerr: %i syserr: %i retry: %i\n",
                          sockfd, nbytes, err, code, syserr, retry);
            Sys_LogError(buf);
//            Sys_LogError(ERR_error_string(code, NULL));
        }
    }

#ifdef _WIN32
    if (err == -1 && WSAGetLastError() != WSAEWOULDBLOCK)
    {
        char buf[1024];
        ERR_error_string(ERR_peek_error(), buf);
    }
#endif

    return err;
}


int ssl_pending(int sockfd)
{
    SingleLock l(&g_ssl_mutex);
    if (!l.lock())
        return -1;

    int err = 0;

    SSL* ssl = lookup_ssl(sockfd);
    if (ssl)
        err = SSL_pending(ssl);
    else
    {   
        char buf[100];
        sprintf(buf, "No SSL for socket %d\n", sockfd);
        Sys_LogError(buf);
        return -1;
    }

    return err;
}


int ssl_check_nbio(int sockfd, int err)
{
    SSL* ssl = lookup_ssl(sockfd);
    if (ssl)
    {
        int code = SSL_get_error(ssl, err);

        static int c = 10;
        if (c > 0)
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

#ifdef _WIN32
        if (WSAGetLastError() == WSAEWOULDBLOCK)
        {   // ok, data has not arrived yet -
            // next iteration will get it
            return 0;
        }
        else
        {   // connection has died - return error
            return -1;
        }
#else
        if (errno == EAGAIN)
        {   // ok, data has not arrived yet -
            // next iteration will get it
            return 0;
        }
        else
        {   // connection has died - return error
            return -1;
        }
#endif

/*
        if (code == SSL_ERROR_NONE ||
            code == SSL_ERROR_WANT_READ)
        {   // Next iteration will read it
            return 0;
        }
        else
            return -1;
*/
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
        return -1;
    }
}

int ssl_read(int sockfd, char* buffer, int nbytes)
{
    SingleLock l(&g_ssl_mutex);
    if (!l.lock())
        return -1;

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
    SingleLock l(&g_ssl_mutex);
    if (!l.lock())
        return -1;

    SSL* ssl = lookup_ssl(sockfd);
    if (ssl)
    {
        int bytesWritten = SSL_write(ssl, buffer, nbytes);
        if (bytesWritten != nbytes)
        {
          static unsigned int i = 0;
          if (i++ < 30)
          {
            char buf[200];
            sprintf(buf, "Partial write: %i %i %i %i",
                    nbytes, bytesWritten,
                    SSL_get_error(ssl, bytesWritten),
                    Sys_GetLastError());
            Sys_LogError(buf);
          }
        }
        return bytesWritten;
    }
    else
    {   
        printf("No SSL for socket %d\n", sockfd);
        return -1;
    }
}

// This is the client SSL context used for
// connection to the lounge
static SSL_CTX* g_ssl_ctx_lounge = NULL;


int ssl_connect(int sd)
{
  SingleLock l(&g_ssl_mutex);
  if (!l.lock())
      return -1;

  // Create separate SSL context for lounge connection.
  // This is because in the table - lounge connection the
  // table is the *client* and the SSL context has to be
  // created with the *client* method.
  if (g_ssl_ctx_lounge == NULL)
    g_ssl_ctx_lounge = SSL_CTX_new(SSLv2_client_method());

  if (g_ssl_ctx == NULL)
    return -1; 

  SSL* ssl = SSL_new(g_ssl_ctx_lounge);
  SSL_set_fd(ssl, sd);

  int err = SSL_connect(ssl);
  if (err == -1)
  {
      Sys_LogError("SSL_connect to lounge fails");
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

int ssl_close(int sockfd)
{
    SingleLock l(&g_ssl_mutex);
    if (!l.lock())
        return -1;

    closesocket(sockfd);

    SSL* ssl = lookup_ssl(sockfd);
    if (ssl)
    {
        SSL_free(ssl);
        g_socket2SSL.erase(sockfd);
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

    if (g_ssl_ctx_lounge)
    {
        SSL_CTX_free(g_ssl_ctx_lounge);
        g_ssl_ctx_lounge = NULL;
    }

    thread_cleanup();
    return 0;
}

// Return true if socket has an SSL association.
bool ssl_checksocket(int sockfd)
{
    SingleLock l(&g_ssl_mutex);
    if (!l.lock())
        return false;

    return lookup_ssl(sockfd) != NULL;
}
