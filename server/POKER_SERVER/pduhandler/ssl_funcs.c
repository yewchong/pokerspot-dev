#include "ssl_funcs.h"

#define CHK_SSL(err) if ((err)==-1) { ERR_print_errors_fp(stderr); fflush(stderr); }

typedef struct socket_ssl
{
  int socket;
  SSL* pssl;
} socket_ssl_t;

//
// Allocate space for 10 simultaneous connections.
// NOTE: this is a temporary workaround for
// now and will be replaced with a hash table.
//
socket_ssl_t g_socket_assocs[10] =
{ {-1, NULL}, {-1, NULL}, {-1, NULL}, {-1, NULL}, {-1, NULL},
  {-1, NULL}, {-1, NULL}, {-1, NULL}, {-1, NULL}, {-1, NULL} };

#define NUMOF(x) (sizeof((x)) / sizeof((x[0])))

int add_assoc(int sd, SSL* ssl)
{
  int i = 0;
  for (i = 0; i < NUMOF(g_socket_assocs); i++)
  {
    if (g_socket_assocs[i].socket == -1)
    {
      g_socket_assocs[i].socket = sd;
      g_socket_assocs[i].pssl = ssl;
      return 0;
    }
  }

  printf("Error: no room for connections\n");
  return -1;
}

SSL* lookup_assoc(int sd)
{
  int i = 0;
  for (i = 0; i < NUMOF(g_socket_assocs); i++)
  {
    if (g_socket_assocs[i].socket == sd)
      return g_socket_assocs[i].pssl;
  }
  return NULL;
}


int Pwrite(int connfd,char *buffer,int length)
{
  if(SSLON) 
  {
    SSL* ssl = lookup_assoc(connfd);
    if (!ssl)
    {
      printf("Error: no SSL for socket: %d\n", connfd);
      return -1;
    }
    return(SSL_write(ssl,buffer,length));
  }
  else
    return(write(connfd,buffer,length));
}


int Pread(int connfd,char *buffer,int length)
{
  if(SSLON)
  {
    SSL* ssl = lookup_assoc(connfd);
    if (!ssl)
    {
      printf("Error: no SSL for socket: %d\n", connfd);
      return -1;
    }
    return(SSL_read(ssl,buffer,length));
  }
  else
  {
    return(read(connfd,buffer,length));
  }    
}

//
// Do the SSL handshaking and set up
// the socket -> SSL association.
//
int Paccept(int connfd, SSL_CTX* ctx)
{
  int err = 0;

  if (SSLON)
  {
    SSL* ssl=SSL_new(ctx);
    if(ssl == NULL)
      printf("little ssl == NULL\n");
    SSL_set_fd(ssl,connfd);
    err = SSL_accept(ssl);
    CHK_SSL(err);

    if (err != -1)
    { // Add (socket, ssl) pair to assocs
      add_assoc(connfd, ssl);
    }
    else
    {
      SSL_free(ssl);
    }
  }

  return err;
}

//
// Remove the socket -> SSL association,
// shutdown the SSL connection and close
// the socket.
//
int Pclosesocket(int sd)
{
  int i = 0, j = 0;

  close(sd);

  for (i = 0; i < NUMOF(g_socket_assocs); i++)
  {
    if (g_socket_assocs[i].socket == sd)
    {
      //SSL_shutdown(g_socket_assocs[i].pssl);
      SSL_free(g_socket_assocs[i].pssl);
      g_socket_assocs[i].pssl = NULL;
      g_socket_assocs[i].socket = -1;
    }
  }

  printf("closed connection to sockfd=%d\n", sd);

  // Remove the socket from the global FD_SET
  // so the main loop will not continue polling
  // this socket.
  FD_CLR(sd, &allset);

  // Remove the socket from the global client[]
  // array so the main loop will not continue trying
  // to read from this socket
  for (j = 0; j < FD_SETSIZE; j++)
  {
    if (client[j] == sd)
      client[j] = -1;
  }

  return 1;
}


//
// Return 1 if a pdu has arrived to socket
// (ie the whole pdu is ready to be read),
// otherwise return -1.
//
int Ppeek(int sd)
{
  char buffer[OUTBUFFERSIZE];
  struct pdu_header pduh;
  memset(&pduh, 0, sizeof(pduh));
  memset(buffer, 0, sizeof(buffer));

  if (SSLON)
  { //
    // peek with SSL
    //
    SSL* ssl = lookup_assoc(sd);
    if (!ssl)
    {
      printf("Ppeek: Error: no ssl for socket %d\n", sd);
      return -1; // should never happen
    }

    if (SSL_peek(ssl, (char*)&pduh, sizeof(pduh)) == sizeof(pduh))
    { // ok, header has arrived - now check the rest of the pdu
      u_int16_t length = ntohs(pduh.length);

      if (length > OUTBUFFERSIZE)
      {
        printf("Ppeek: Error: buffer too small\n");
        return -1;
      }

      if (length > sizeof(pduh))
      {
        if (SSL_peek(ssl, buffer, length) == length)
        { // ok, the whole pdu has arrived
          return 1; 
        }
      }
      else
      { // ok, the whole pdu has arrived
        return 1;
      }
    }
  }
  else
  { //
    // peek with bare socket
    //
    if (recv(sd, &pduh, sizeof(pduh), MSG_PEEK) == sizeof(pduh))
    { // ok, header has arrived - now check the rest of the pdu
      u_int16_t length = ntohs(pduh.length);

      if (length > OUTBUFFERSIZE)
      {
        printf("Ppeek: Error: buffer too small\n");
        return -1;
      }

      if (length > sizeof(pduh))
      {
        if (recv(sd, buffer, length, MSG_PEEK) == length)
        { // ok, the whole pdu has arrived
          return 1; 
        }
      }
      else
      { // ok, the whole pdu has arrived
        return 1;
      }
    }
  }

  // part of pdu is still missing
  return -1;
}
