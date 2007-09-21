/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class Server
  Created  : 04/15/2000

  OVERVIEW : Lounge server.

 ****************************************************************************/

#include "server.h"
#include "sys.h"
#include "def.h"
#include "poller.h"
#include "pinger.h"
#include "lounge/lounge.h"
#include "lounge/queueservice.h"
#include "lounge/game.h"
#include "lounge/tester.h"
#include "loadbalancer/loadbalancer.h"
#include "dbhandler/dbhandler.h"
#include "dbhandler/dbinterface.h"
#include "pdu/pdunotify.h"
#include "ssl_funcs.h"
#include <string>


namespace
{

  // Define a function to be called if new fails to allocate memory.
  int psi_new_handler(size_t size)
  {
    Sys_LogError("FATAL: CServer out of memory.\n");
    // XXX thow out of memory exception
    return 0;
  }

  // Handle control-c by setting up Quit-flag to
  // allow cleanup on server exit
  BOOL WINAPI CtrlCHandler(DWORD dwCtrlType)
  {
    Sys_LogError("CTRL-C pressed, exiting...\n\n");
    Server::Inst()->quit(true);
    return TRUE;
  }

}


Server* Server::pInstance_ = NULL;

Server::Server()
  :
  sd_player_(INVALID_SOCKET),
  sd_table_ (INVALID_SOCKET),
  port_     (0),
  quit_     (FALSE)
{
  pInstance_ = this;
}


Server::~Server()
{
  pInstance_ = NULL;
}


int Server::initialize(int argc, char* argv[])
{
  SetConsoleCtrlHandler(CtrlCHandler, TRUE);

  // Catch memory allocation failures 
  _set_new_handler(psi_new_handler);
  
  if (!Sys_InitSocket())
  {
    Sys_LogError("InitSockets failed");
    return -1;
  }
  
  // Make contact with the database - if it
  // fails it makes no sense to continue
  CdbHandler* dbase = new CdbHandler(DBMS_TYPE_MYSQL);
  if (!dbase)
  {
    Sys_LogError("Out of memory: new CdbHandler");
    return -1;
  }
  
  if (!dbase->dbConnect(DATABASE_SERVER,
                        DATABASE_USER,
                        DATABASE_PASSWORD,
                        DATABASE))
  {
    delete dbase;
    Sys_LogError("Can't connect to database.", LOG_FLAG_DATABASE);
    Sys_Sleep(2000);
    return -1;
  }
  
  CLounge::Inst()->setDbase(dbase);
  
  // Initialize SSL - run without SSL if last argument
  // is -nossl
  if (argc > 1 && strcmp(argv[1], "-nossl") == 0)
    Sys_DisableSSL();
  else
    Sys_EnableSSL();
  
  if (Sys_IsSSLEnabled() && init_ssl() == -1)
  {
    Sys_LogError("Can't initialize SSL");
    return -1;
  }

  // Decide which port we'll be listening to
  port_ = PSPOT_LOUNGESERVER_PORT;  
  if (argc > 2)
  {
    port_ = atoi(argv[2]);
    if (port_ == 0)
      port_ = PSPOT_LOUNGESERVER_PORT;
  }
  
  if (readConfig() == -1)
  {
    return -1;
  }
   
  struct sockaddr_in addr_srv;
  memset(&addr_srv, 0, sizeof(addr_srv));
  
  //
  // Create player socket
  //
  sd_player_ = socket(AF_INET, SOCK_STREAM, 0);
  if (sd_player_ == INVALID_SOCKET)
  {
    Sys_LogError("socket player failed");
    return -1;
  }
  
  // Bind will fail without this if server is restarted soon 
  int opt = 1;	
  setsockopt(sd_player_, SOL_SOCKET, SO_REUSEADDR,
             (const char*)&opt, sizeof(opt));
  
  addr_srv.sin_family = AF_INET;
  addr_srv.sin_addr.s_addr = htons(INADDR_ANY);
  addr_srv.sin_port = htons(port_);
  
  int err = bind(sd_player_,
                 (const struct sockaddr*)&addr_srv,
                 sizeof(addr_srv));
  
  if (err == INVALID_SOCKET)
  {
    Sys_LogError("bind player socket failed");
    return -1;
  }
  
  err = listen(sd_player_, SOMAXCONN);
  if (err == INVALID_SOCKET)
  {
    Sys_LogError("listen player socket failed");
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

    if (err == -1)
    {
      Sys_LogError("setsockopt SO_RCVTIMEO fails");
    }
  }
  
  //
  // Create table socket
  //
  sd_table_ = socket(AF_INET, SOCK_STREAM, 0);
  if (sd_table_ == INVALID_SOCKET)
  {
    Sys_LogError("socket table failed");
    return -1;
  }
  
  addr_srv.sin_family = AF_INET;
  addr_srv.sin_addr.s_addr = htons(INADDR_ANY);
  addr_srv.sin_port = htons(port_ + 1);
  
  err = bind(sd_table_,
             (const struct sockaddr*)&addr_srv,
             sizeof(addr_srv));
  
  if (err == INVALID_SOCKET)
  {
    Sys_LogError("bind table socket failed");
    return -1;
  }
  
  err = listen(sd_table_, SOMAXCONN);
  if (err == INVALID_SOCKET)
  {
    Sys_LogError("listen table socket failed");
    return -1;
  }

  if (CWG_HANDSHAKE_TIMEOUT > 0)
  {
    int ssl_handshake_timeout = CWG_HANDSHAKE_TIMEOUT;
    err = setsockopt(sd_table_,
                     SOL_SOCKET,
                     SO_RCVTIMEO,
                     (const char*)&ssl_handshake_timeout,
                     sizeof(ssl_handshake_timeout));

    if (err == -1)
    {
      Sys_LogError("setsockopt SO_RCVTIMEO fails");
    }
  }
           
  //
  // Process configuration
  //
  if (processConfig(port_ + 1) == -1)
  {
    return -1;
  }

 
  printf("\nPokerSpot Lounge server started at %s", Sys_GetDate());
  printf("Player port: %d table port: %d, ", port_, port_ + 1);

  if (Sys_IsSSLEnabled())
    printf("SSL security enabled\n");
  else
    printf("SSL security disabled\n");
  
  return 1;
}

/*
 * int Server::mainLoop
 *
 * This is the server main loop.
 */
int Server::mainLoop()
{
    int rc = 0;
    long now = 0;

    CPoller poller(sd_player_, sd_table_);
    CPinger pinger;
    CTester tester; // This object adds bots to the waiting lists

    while (!quit_)
    {
        time(&now);
        tester.tick(now);

        rc = poller.poll();

        CLounge::Inst()->tick(now);
        CQueueService::ServiceQueues(now);
        pinger.tick(now);

        checkShutdown();

        // Exit if we're shutting down and no tables left
        if (CLounge::Inst()->getShutdown() != 0 &&
            CLounge::Inst()->numTables() == 0)
        {
            quit_ = true;
        }
    }
  
    return rc;
}


void Server::shutdown()
{
  delete CLoadBalancer::Inst();

  if (sd_player_ != INVALID_SOCKET)
  {
    Sys_CloseSocket(sd_player_);
    sd_player_ = INVALID_SOCKET;
  }
  
  if (sd_table_ != INVALID_SOCKET)
  {
    Sys_CloseSocket(sd_table_);
    sd_table_ = INVALID_SOCKET;
  }
  
  if (Sys_IsSSLEnabled())
    ssl_shutdown();
  
  Sys_ShutdownSocket();
  
  printf("PokerSpot lounge server shut down at %s\n", Sys_GetDate());
}


void Server::controlBreak()
{
  quit_ = true;
}

bool Validate(const char* add,
              const char* port)
{
    return true;
}

bool Validate(const char* tabletype,
              const char* tablemax,
              const char* tablelow,
              const char* tablehigh,
              const char* spawnthreshold,
              const char* initspawn)
{
    return true;
}

int Server::readConfig()
{
    int rc = 0;

    printf("Reading server configuration...");

    FILE* fd = NULL;
    const int RBUFSIZE = 512;
    char readbuff[RBUFSIZE+1];	

    fd = fopen("master.conf", "r");
    if (!fd)
    {
        Sys_LogError("Can't open master configuration file: master.conf");
        return -1;
    }
    while (fgets(readbuff, RBUFSIZE, fd))
    {
        if (readbuff[0] == '#')
            continue;

        const char* addr = strtok(readbuff, ":");
        const char* port = strtok(NULL, ":");

        if (!Validate(addr, port))
        {
            rc = -1;
            string s("Invalid entry in master.conf: ");
            const string d(",");
            s += (addr + d + port);
            Sys_LogError(s.c_str());
            break;
        }

        if (!CLoadBalancer::ConnectToLoadBalancer(inet_addr(addr), atoi(port)))
        {
            string s("Can't connect to Load Balancer: ");
            const string d(",");
            s += (addr + d + port);
            Sys_LogError(s.c_str());
        }
    }
    fclose(fd);


    fd = fopen("waitinglists.conf", "r");
    if (!fd)
    {
        Sys_LogError("Can't open table configuration file: table.conf");
        return -1;
    }

    int queueCounter = 0;

    while (fgets(readbuff, RBUFSIZE, fd))
    {
        if (strlen(readbuff) <= 1)
        {   // empty line
            continue;
        }

        if (readbuff[0] == '#')
        {   // comment
            continue;
        }

        if (readbuff[0] == '@')
        {   // Game name
            char* b = &(readbuff[1]);
            const char* gametype = strtok(b, ":");
            const char* gamename = strtok(NULL, ":");
            CGame::AddGame(atoi(gametype), gamename);
            continue;
        }

        const char* tabletype = strtok(readbuff, ":");
        const char* queuemax = strtok(NULL, ":");
        const char* tablemax = strtok(NULL, ":");
        const char* tablelow = strtok(NULL, ":");         
        const char* tablehigh = strtok(NULL, ":");
        const char* spawnthreshold = strtok(NULL, ":");
        const char* initspawn = strtok(NULL, ":");

        if (!Validate(tabletype, tablemax,
                      tablelow, tablehigh,
                      initspawn, spawnthreshold))
        {
            rc = -1;
            string s("Invalid table entry in table.conf: ");
            const string d(",");
            s += (tabletype + d + tablemax + d + tablelow + d +
                  tablehigh + d + spawnthreshold + d + initspawn);
            Sys_LogError(s.c_str());
            break;
        }

        if (!CLounge::Inst()->addWaitingList(
                                queueCounter++,
                                atoi(tabletype),
                                atoi(queuemax),
                                atoi(tablemax),
                                atoi(tablelow),
                                atoi(tablehigh),
                                atoi(spawnthreshold),
                                atoi(initspawn)))
        {
            rc = -1;
            string s("Invalid table entry in table.conf: ");
            const string d(",");
            s += (tabletype + d + tablemax + d + tablelow + d +
                  tablehigh + d + spawnthreshold + d + initspawn);
            Sys_LogError(s.c_str());
            break;
        }

    }
    fclose(fd);

    printf(" done\n");

    return rc;
}

int Server::processConfig(u_short tableport)
{
    int rc = 0;

    // Print out the current configuration
    CLounge::Inst()->dumpConfig();

    return rc;
}


void Server::checkShutdown()
{
    static int i = 0;
    if (i++ == 40)
    {   // Check shutdown every N ticks
        i = 0;
        if (CLounge::Inst()->getShutdown() == 0)
        {
            u_int16_t shutdown = 0;
            CdbInterface::Inst()->getShutdown(shutdown);
            CLounge::Inst()->setShutdown(shutdown);
            
            if (shutdown != 0)
            {
                string msg;
                CdbInterface::Inst()->getShutdownMsg(msg);
                CpduNotify pdu;
                if (msg.size() != 0)
                    pdu.sendAllNotifyMessage(msg.c_str());
                else
                    pdu.sendAllNotifyMessage("PokerSpot server will be shut down for maintenance.\n\nThank You for playing at PokerSpot!");
                
                // Close all player sockets now
                
                for (PlayerIterator p = CLounge::Inst()->allPlayers();
                     p.more(); p.next())
                {
                    if (p.current()->getSocket() != -1)
                        Sys_CloseSocket(p.current()->getSocket());
                }
                
            }
        }
    }
}
