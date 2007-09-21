#include "pokerserv.h"
#include "gamemain.h"
#include "def.h"
#include "sys.h"
#include "gettime.h"
#include "plm/plm.h"
#include "plm_7stud/plm_7stud.h"
#include <new.h>


// Global Variables Defined Here
fd_set rset, allset;
ThreadInfo g_ThreadInfo;

int main(int argc, char *argv[])
{
  // Catch and handle memory allocation failures 
#ifdef _WIN32
  _set_new_handler(psi_new_handler);
#else
  set_new_handler(psi_new_handler);
  signal(SIGPIPE, SIG_IGN);
#endif
  
  CTable table;
  CPoller poller;
  Cplm* plm = NULL; 
  CdbHandler *dbase = NULL;
  
  srand(time(NULL));
  
  u_short port = DEFAULT_TABLE_PORT;

  /*
  * Command line arguments are as follows:
  * - game type
  * - queue index (for lounge)
  * - table number (== seed)
  * - max number of players 
  * - low limit
  * - hi limit
  * - lounge server ip address
  * - lounge server port
  * - my port
  */
  if (argc < 11)
  {
    printf("Usage: %s <game type> <queue index> <table number> <table max> <lo limit> <hi limit>\n"
           "       <lounge server ip> <lounge server port> <table port>\n", argv[0]);
    printf("Running stand-alone with default settings.\n");
    
    table.setLo(DEFAULT_LOW);
    table.setHi(DEFAULT_HI);
    table.setSeed(1);
    table.setGameType(Game_Type_Holdem);
    table.setMaxPlayers(10);
  }
  else
  {
    table.setGameType(atoi(argv[10]));
    table.setQueueIndex(atoi(argv[1]));
    table.setSeed(atoi(argv[2]));
    table.setMaxPlayers(atoi(argv[3]));
    table.setLo(atoi(argv[4]));
    table.setHi(atoi(argv[5]));
    port = atoi(argv[8]);
  }

  switch (table.getGameType())
  {
  default:
  case Game_Type_Holdem:
    plm = new Cplm(&table);
    break;
    
  case Game_Type_SevenStud:
    plm = new Cplm_7Stud(&table);
    break;
  }
  g_ThreadInfo.plm_ = plm;

  if (argc > 9)
    plm->setHiLoSplit((atoi(argv[9]) == 0) ? false : true);
  
  printf("Table: seed: %i type: %i %s max: %i lo: %9.2f hi: %9.2f\n",
         table.getSeed(), table.getGameType(),
         plm->isHiLoSplit() ? "HILO" : "LIMIT",
         table.getMaxPlayers(),
         table.getLo().asDouble(),
         table.getHi().asDouble());
  
  char s[100];
  sprintf(s, "errorlog-%d.txt", table.getSeed());
  Sys_InitErrorLog(s);
  sprintf(s, "chiplog-%d.txt", table.getSeed());
  Sys_InitChipLog(s);

	struct timeval *pdu_wait = NULL;
	struct sockaddr_in *servAddr = NULL;

	dbase = new CdbHandler(DBMS_TYPE_MYSQL);
	if (!dbase->dbConnect(DATABASE_SERVER, DATABASE_USER, DATABASE_PASSWORD, DATABASE))
  {
    delete dbase;
    // Database initialization failed, makes no sense to continue
    return -1;
  }

  servAddr = (sockaddr_in*)malloc(sizeof(struct sockaddr_in));

  table.setDbase(dbase);

	servAddr->sin_port = htons(port);
	servAddr->sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr->sin_family = AF_INET;

  // SSL will be enabled if last argument is -ssl
  // BY DEFAULT SSL IS DISABLED
  bool ssl = false;
  if (argc > 1)
    ssl = (strcmp(argv[argc-1], "-ssl") == 0);
  
  int rc = poller.init(argc, argv, servAddr, &table, ssl); 
  if (rc == -1)
  {   // Initialization failed, makes no sense to continue
    return -1;
  }
  
  Sys_CreateThread(GameMain, &g_ThreadInfo);
  
  printf("PokerSpot server at port %d, started %s",
         DEFAULT_TABLE_PORT, GetTime());
  if (Sys_IsSSLEnabled())
    printf("SSL security enabled\n");
  else
    printf("SSL security DISABLED\n");

	pdu_wait = (struct timeval *)malloc(sizeof(struct timeval));
	bzero(pdu_wait, sizeof(struct timeval));

	pdu_wait->tv_sec = 1;

	bool shutdown = false;


  try
  {
    while (!shutdown)
	  {
      poller.poll(pdu_wait);
    
      long now = time(NULL);
      table.tick(now);

      static int countdown = 0;
      if (countdown == 0)
      {
	      countdown = table.getShutdown();

        if (countdown > 0)
        {
          printf("Exit condition, shutting down... ");
          // end the game logic thread
          g_ThreadInfo.quit_ = true;
        }
      }
      if (countdown > 0)
      {
        countdown--;
        printf("%d ", countdown); fflush(stdout);
        if (countdown <= 1)
        {
          shutdown = true;
          printf("\nServer going down\n");
        }
      }
    }
  }
  catch(const FatalRuntimeError&)
  {
    Sys_LogError("Runtime exception in main thread!\n");
    if (CTable::Instance())
      CTable::Instance()->setShutdown(1);
  }

  // Inform lounge server we're going down
  poller.sendTableLogout();

  // Sleep for a while to make sure the
  // game logic thread has gone
  g_ThreadInfo.quit_ = true;
  Sys_Sleep(5000);

  poller.shutdown();

	// Upon Exiting, Do Teardown.
	free(servAddr);
	dbase->dbClose();
  delete dbase;

  printf("PokerSpot server shut down at %s", GetTime());
  delete plm;
  return 0;
};
