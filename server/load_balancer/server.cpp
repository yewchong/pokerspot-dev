/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CServer
  Created  : 09/21/2000

  OVERVIEW : CServer class contains the main loop of the Load Balancer.

 ****************************************************************************/

#include "server.h"
#include "poller.h"
#include "def.h"
#include "common/errorlog.h"
#include "common/sys.h"
#include "common/ssl_funcs.h"
#include <new.h>


namespace
{

  // Define a function to be called if new fails to allocate memory.
  int psi_new_handler(size_t size)
  {
    Err::Log() << "FATAL: CServer out of memory.\n";
    // XXX thow out of memory exception
    return 0;
  }

  // Handle control-c by setting up Quit-flag to
  // allow cleanup on server exit
  BOOL WINAPI CtrlCHandler(DWORD dwCtrlType)
  {
    Err::Log() << "CTRL-C pressed, exiting..." << CLog::nl << CLog::nl;
    CServer::Inst()->setQuit(true);
    return TRUE;
  }

}

CServer* CServer::pInst_ = NULL;


CServer::CServer()
  :
  sd_lounge_(INVALID_SOCKET),
  sd_player_(INVALID_SOCKET),
  quit_     (false),
  ipaddress_(0)
{
  pInst_ = this;
}


CServer::~CServer()
{
  pInst_ = NULL;
}


int CServer::initialize(int argc, char** argv)
{
  SetConsoleCtrlHandler(CtrlCHandler, TRUE);
 
  // Catch memory allocation failures 
  _set_new_handler(psi_new_handler);

  // Disable SSL if '-nossl' is the last command line argument
  if (argc > 1 && strcmp(argv[argc-1], "-nossl") == 0)
  {
    Sys_SetSSLEnabled(FALSE);
  }
  else
  {
    Sys_SetSSLEnabled(TRUE);
  }

  if (readConfig() == -1)
  {
    return -1;
  }

  if (Sys_InitSocket() < 0)
  {
    return -1;
  }

  //
  // Create Lounge & Player sockets
  //

  struct sockaddr_in addr_srv;
  memset(&addr_srv, 0, sizeof(addr_srv));
  u_short port = CWG_LOADBALANCER_PORT;

  if (argc > 2)
  {
    port = atoi(argv[2]);
    if (port == 0)
      port = CWG_LOADBALANCER_PORT;
  }

  //
  // Create lounge socket
  //
  sd_lounge_ = socket(AF_INET, SOCK_STREAM, 0);
  if (sd_lounge_ == INVALID_SOCKET)
  {
    Err::Log() << "Failed to create lounge socket.\n";
    return -1;
  }

  // Bind will fail without this if server is restarted soon
  // - we don't really care if this fails
  int opt = 1; 
  int err = setsockopt(sd_lounge_, SOL_SOCKET, SO_REUSEADDR,
                       (const char*)&opt, sizeof(opt));
  
  addr_srv.sin_family = AF_INET;
  addr_srv.sin_addr.s_addr = htons(INADDR_ANY);
  addr_srv.sin_port = htons(port);

  err = bind(sd_lounge_,
             (const struct sockaddr*)&addr_srv,
             sizeof(addr_srv));

  if (err == INVALID_SOCKET)
  {
    Err::Log() << "Failed to bind lounge socket.\n";
    return -1;
  }

  err = listen(sd_lounge_, SOMAXCONN);
  if (err == INVALID_SOCKET)
  {
    Err::Log() << "Failed to listen lounge socket.\n";
    return -1;
  }


  //
  // Set SSL handshake timeout
  //
  if (CWG_HANDSHAKE_TIMEOUT > 0)
  {
    int ssl_handshake_timeout = CWG_HANDSHAKE_TIMEOUT;
    err = setsockopt(sd_lounge_,
                     SOL_SOCKET,
                     SO_RCVTIMEO,
                     (const char*)&ssl_handshake_timeout,
                     sizeof(ssl_handshake_timeout));

    if (err < 0)
    {
      Err::Log() << "setsockopt SO_RCVTIMEO fails (lounge).\n";
    }
  }
  

  //
  // Create player socket
  //
  sd_player_ = socket(AF_INET, SOCK_STREAM, 0);
  if (sd_player_ == INVALID_SOCKET)
  {
    Err::Log() << "Failed to create player socket.\n";
    return -1;
  }

  // Bind will fail without this if server is restarted soon 
  opt = 1; 
  err = setsockopt(sd_player_, SOL_SOCKET, SO_REUSEADDR,
                   (const char*)&opt, sizeof(opt));

  addr_srv.sin_family = AF_INET;
  addr_srv.sin_addr.s_addr = htons(INADDR_ANY);
  addr_srv.sin_port = htons(port + 1);
  
  err = bind(sd_player_,
             (const struct sockaddr*)&addr_srv,
             sizeof(addr_srv));

  if (err == INVALID_SOCKET)
  {
    Err::Log() << "Failed to bind player socket.\n";
    return -1;
  }

  err = listen(sd_player_, SOMAXCONN);
  if (err == INVALID_SOCKET)
  {
    Err::Log() << "Failed to listen player socket.\n";
    return -1;
  }

  if (CWG_HANDSHAKE_TIMEOUT > 0)
  {
    int ssl_handshake_timeout = CWG_HANDSHAKE_TIMEOUT;
    err = setsockopt(sd_player_,
                     SOL_SOCKET,
                     SO_RCVTIMEO,
                     (const char*)&ssl_handshake_timeout,
                     sizeof(ssl_handshake_timeout));

    if (err < 0)
    {
      Err::Log() << "setsockopt SO_RCVTIMEO fails (player).\n";
    }
  }

  //
  // Figure out local host's ip address
  //
  struct sockaddr_in cliaddr;
  memset(&cliaddr, 0, sizeof(cliaddr));
  int clilen = sizeof(cliaddr);

  struct hostent* phostent = NULL;
  char hostname[MAXHOSTNAMELEN+1];
  if (gethostname(hostname, MAXHOSTNAMELEN) < 0)
  {
    Err::Log() << "gethostname failed.\n";
    return -1;
  }

  phostent = gethostbyname(hostname);      
  if (!phostent)
  {
    Err::Log() << "gethostbyname(" << hostname << ") failed.\n";
    return -1;
  }

  memcpy(&cliaddr.sin_addr, phostent->h_addr_list[0], phostent->h_length);    
  u_long addr1 = cliaddr.sin_addr.s_addr;
  ipaddress_ = ntohl(addr1);

  return 1;
}


/*
 * int CServer::mainLoop
 *
 * This is the CServer main loop.
 */
int CServer::mainLoop()
{
  CPoller poller(sd_player_, sd_lounge_);

  int rc = 0;
  DWORD ticks1 = 0,
        ticks2 = 0;
 
  while (!quit_)
  {
    ticks1 = GetTickCount();

    rc = poller.poll();

    ticks2 = GetTickCount();

    DWORD diff = ticks2 - ticks1;

    // XXX if diff < X sleep (X - diff)
  }
  
  return rc;
}


void CServer::shutdown()
{
  if (sd_lounge_ != INVALID_SOCKET)
  {
    Sys_closesocket(sd_lounge_);
    sd_lounge_ = INVALID_SOCKET;
  }

  if (sd_player_ != INVALID_SOCKET)
  {
    Sys_closesocket(sd_player_);
    sd_player_ = INVALID_SOCKET;
  }
    
  Sys_ShutdownSocket();
}


int CServer::readConfig()
{
  int rc = 0;
  return rc;
}


int CServer::processConfig()
{
  int rc = 0;
  return rc;
}


u_long CServer::getLocalHostIpAddress() const
{
  return ipaddress_;
}
