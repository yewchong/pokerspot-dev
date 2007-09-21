/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : sslfuncs.h
  Created  : 04/24/2000

  OVERVIEW : SSL functions for storing and retrieving socket -> SSL
             associations.

             <socket,SSL*> pairs are stored into a hash table, lookup
             is constant time.

 ****************************************************************************/

#include "common/ssl_funcs.h"
#include "common/sys.h"
#include "common/errorlog.h"
#include "common/sync.h"
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/err.h> 
#include <map>
#include <iostream.h>
USING_NS_T(std, map);

#define CERTF "server.pem"
#define KEYF  "server.pem"

// Link in the SLL libraries
#ifdef _WIN32
  #pragma comment(lib, "..\\..\\lib\\openssl\\out32dll\\ssleay32.lib")
  #pragma comment(lib, "..\\..\\lib\\openssl\\out32dll\\libeay32.lib")
#endif


// SSL Thread protection

static HANDLE lock_cs[CRYPTO_NUM_LOCKS];
void ssl_thread_locking_callback(int mode, int type, const char* file, int line);
unsigned long ssl_thread_id();

void ssl_thread_setup()
{ 
  for (int i = 0; i < CRYPTO_NUM_LOCKS; i++)
  {
    lock_cs[i] = Sys_CreateMutex();
  }
  
  CRYPTO_set_id_callback((unsigned long (*)())ssl_thread_id);
  CRYPTO_set_locking_callback(ssl_thread_locking_callback);
}

void ssl_thread_cleanup()
{ 
  CRYPTO_set_locking_callback(NULL);
  for (int i = 0; i < CRYPTO_NUM_LOCKS; i++)
  {
    Sys_DestroyMutex(lock_cs[i]);
    lock_cs[i] = NULL;
  }
}

void ssl_thread_locking_callback(int mode, int type, const char* file, int line)
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

unsigned long ssl_thread_id()
{
  unsigned long ret = (unsigned long)GetCurrentThreadId();
  return ret;
}


// This is the server SSL context used for
// incoming connections (i.e., "server" side).
static SSL_CTX* g_ssl_ctx = NULL;

int init_ssl()
{
  SSL_load_error_strings();
  SSLeay_add_ssl_algorithms();
  g_ssl_ctx = SSL_CTX_new(SSLv23_server_method());
  
  if (!g_ssl_ctx)
  {
    Err::Log() << "SSL_CTX_new failed\n";
    return -1;
  }
  
  if (SSL_CTX_use_certificate_file(g_ssl_ctx, CERTF, SSL_FILETYPE_PEM) < 0)
  {
    Err::Log() << "SSL cert load failed\n";
    SSL_CTX_free(g_ssl_ctx);
    g_ssl_ctx = NULL;
    return -1;
  }
  
  if (SSL_CTX_use_PrivateKey_file(g_ssl_ctx, KEYF, SSL_FILETYPE_PEM) < 0)
  {
    Err::Log() << "SSL rsa key load failed\n";
    SSL_CTX_free(g_ssl_ctx);
    g_ssl_ctx = NULL;
    return -1;
  }
  
  ssl_thread_setup();
  
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

// Add association between socket -> SSL.
// Return true if association was added, return false
// if association was already stored.
bool add_socket_ssl_assoc(int sockfd, SSL* ssl)
{
  return (g_socket2SSL.insert(Socket2SSL::value_type(sockfd, ssl)).second);
}

// PROTECT ALL SSL ACCESS WITH A MUTEX
// This takes also care of potential race
// conditions with the hash table being
// accessed from the two threads.

static Mutex g_ssl_mutex;


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
    Err::Log() << "SSL_new\n";
    return -1;
  }
  
  SSL_set_fd(ssl, sockfd);
  int err = SSL_accept(ssl);
  
  if (err == -1 || err == 0)
  {
    int syserr = Sys_GetLastError();;
    int sslerr = SSL_get_error(ssl, err);
    Err::Log() << "SSL_accept fails: "
               << err << ", " << syserr << ", " << sslerr << CLog::nl;
    SSL_free(ssl);
    return -1;
  }
  
  cout << "Accepted SSL connection using " << SSL_get_cipher(ssl) << CLog::nl;
  
  SingleLock l(&g_ssl_mutex);
  if (!l.lock())
    return -1;
  
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
  SingleLock l(&g_ssl_mutex);
  if (!l.lock())
    return -1;
  
  int err = 0;
  
  SSL* ssl = lookup_ssl(sockfd);
  if (ssl)
    err = SSL_peek(ssl, buffer, nbytes);
  else
  {   
    Err::Log() << "No SSL for socket " << sockfd << '\n';
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
      Err::Log() << "SSL_peek: sock: " << sockfd 
                 << ", nbytes: " << nbytes
                 << ", err: " << err
                 << ", sslerr: " << code
                 << ", syserr: " << syserr
                 << ", retry: " << retry << CLog::nl;
    }
  }
    
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
    Err::Log() << "No SSL for socket " << sockfd << CLog::nl;
    return -1;
  }
  
  return err;
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
    cout << "No SSL for socket " << sockfd << CLog::nl;
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
        Err::Log() << "Partial write: "
                   << nbytes << ',' <<  bytesWritten << ','
                   << SSL_get_error(ssl, bytesWritten) << ','
                   << Sys_GetLastError();
      }
    }
    return bytesWritten;
  }
  else
  {   
    cout << "No SSL for socket " << sockfd << CLog::nl;
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
    Err::Log() << "SSL_connect to lounge fails\n";
    SSL_free(ssl);
    return -1;
  }

  if (add_socket_ssl_assoc(sd, ssl))
    return 0;
  else
  {
    Err::Log() << "Socket " << sd << " has already SSL association";
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
  
  ::closesocket(sockfd);
  
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
  
  ssl_thread_cleanup();
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
