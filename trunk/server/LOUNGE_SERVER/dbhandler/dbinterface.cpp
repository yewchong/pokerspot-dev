/**********************************************************************

  NAME:		dbinterface.cpp
  DATE:		4/9/00
  PURPOSE:	Class CdbInterface implements the functions needed to
			interface with our proprietary database schema.  These
			functions allow us to grab and set information such as
			a player's number of chips, or the game's logs.

**********************************************************************/


#include "dbhandler/dbinterface.h"
#include "dbhandler/dbhandler.h"
#include "dbhandler/md5.h"
#include "lounge/player.h"
#include "pdu/pducreditcardreply.h"
#include "pdu/pducreditcardreply2.h"
#include "tournament/tournamentevent.h"
#include "common/chips.h"
#include "common/stringoutput.h"
#include "sys.h"
#include <stdio.h>




// Put a \ in front of characters that would
// cause an SQL query to fail
static string sqlfy(const char* p)
{
  string s;
  int l = strlen(p);
  for (int i = 0; i < l; i++)
  {
    if (p[i] == '\'' ||
      p[i] == '\"' ||
      p[i] == ';')
    {
      s += '\\';
    }
    s += p[i];
  }
  return s;
}


//
// Return string quoted so it can be used
// in database queries.
//
// NOTE: This function returns a pointer to
// a static character buffer. Subsequent calls
// overwrite the same buffer so the result must
// be copied if it is to be used later.
//
const char* quote(const char* p)
{
  static string store;
  int len = 0;
  if (p != NULL)
    len = strlen(p);

  // Allocate space for quoted string, null and
  // start and end quote
  char* buf = new char[2 * len + 1 + 2];
  if (!buf) return NULL;

  char* b = buf;

  // Append one quote at the beginning
  *buf++ = '\'';
  if (p != NULL)
  {
    mysql_escape_string(buf, p, len);
    len = strlen(buf);
  }
  // Append another quote at the end
  *(buf + len) = '\'';
  *(buf + len + 1) = '\0';

  store = b;

  delete [] b;
  return store.c_str();
}



//
// These two structures can be used for RAII-style
// locking of database tables.
//
struct ReadLockSQLTable
{
  ReadLockSQLTable(CdbHandler* dbase, const char* table)
    :
  dbase_(dbase)
  {
    char query[MAXQUERYSIZE];
    sprintf(query, "LOCK TABLES %s READ", table);
    success_ = dbase_->dbQuery(query);
  }
  
  ~ReadLockSQLTable()
  {
    if (success_)
    {
      char* query = "UNLOCK TABLES";
      dbase_->dbQuery(query);
    }
  }
  CdbHandler* dbase_;
  bool success_;
};

struct WriteLockSQLTable
{
  WriteLockSQLTable(CdbHandler* dbase, const char* table)
    :
  dbase_(dbase)
  {
    char query[MAXQUERYSIZE];
    sprintf(query, "LOCK TABLES %s WRITE", table);
    success_ = dbase_->dbQuery(query);
  }
  
  ~WriteLockSQLTable()
  {
    if (success_)
    {
      char* query = "UNLOCK TABLES";
      dbase_->dbQuery(query);
    }
  }
  CdbHandler* dbase_;
  bool success_;
};



CdbInterface* CdbInterface::pInst_ = NULL;

CdbInterface::CdbInterface(CdbHandler* dbase)
  :
  dbase_(dbase)
{
  pInst_ = this;
}

CdbInterface::~CdbInterface()
{
  delete dbase_;
  dbase_ = NULL;
  pInst_ = NULL;
}


bool CdbInterface::authenticate(const char* username, const char* password)
{
  ReadLockSQLTable sqlLock(dbase_, "pokeruser");
  
  CStrOut query;
  char* dbPassword = NULL;
  
  string u = sqlfy(username);
  
  MD5 context;
  context.update((unsigned char*)password, strlen(password));
  context.finalize();
  
  query << "SELECT password FROM pokeruser WHERE username='"
        << u.c_str() << '\'';
  
  if (!dbase_->dbQuery(query.str()))
  {
    if (DEBUG & DEBUG_DATABASE)
    {
      printf("Query to authenticate %s failed!\n", username);
    }
    
    return false;
  }
  
  dbPassword = dbase_->dbFetchRow();
  
  if (dbPassword == NULL)
  {
    if (DEBUG & DEBUG_DATABASE)
    {
      printf("User %s does not exist in database!\n", username);
    }
    
    return false;
    
  }
  else if (strcmp(dbPassword, context.hex_digest()))
  {
    if (DEBUG & DEBUG_DATABASE)
    {
      printf("User supplied password and password on file don't match!\n");            
    }
    return false;
  }
  
  return true;
};

bool CdbInterface::getChips(const CPlayer* player, CChips& chips)
{
  CStrOut query;
  char* dbFetched = NULL;
  
  string name = sqlfy(player->getUsername());
  
  query << "SELECT chips FROM pokeruser WHERE username='" 
        << name.c_str() << '\'';
  
  if (!dbase_->dbQuery(query.str()))
  {
    if (DEBUG & DEBUG_DATABASE)
    {
      printf("Query to get %s's chips failed!\n", player->getUsername());
    }
    
    return false;
  }
  
  dbFetched = dbase_->dbFetchRow();
  
  if (dbFetched == NULL)
  {
    if (DEBUG & DEBUG_DATABASE)
    {
      printf("User %s does not exist in database!\n", player->getUsername());
    }
    
    return false;
  }
  
  double c = atof(dbFetched);
  chips = CChips(c);
  return true;
};


bool CdbInterface::getCreditCards(const CPlayer* player, list<pdu_creditcard>& cards)
{
  const int MaxCreditCards = 5;
  char query[MAXQUERYSIZE];
  
  string uname = sqlfy(player->getUsername());
  
  memset(query, 0, MAXQUERYSIZE);
  
  for (int i = 1; i < MaxCreditCards; i++)
  {
    pdu_creditcard cc;
    memset(&cc, 0, sizeof(cc));
    
    char column[100];
    sprintf(column, "cc%d_type", i);
    sprintf(query, "SELECT %s FROM customers WHERE username='%s'", column, uname.c_str());
    
    if (!dbase_->dbQuery(query))
    {
      char s[200];
      sprintf(s, "Query to get %s's credit cards failed!\n",
        player->getUsername());
      Sys_LogError(s);
      return false;
    }
    
    char* dbFetched = dbase_->dbFetchRow();
    
    if (dbFetched != NULL)
    {
      strncpy(cc.card_type, dbFetched, PDU_STRINGSIZE - 1);
      
      sprintf(column, "cc%d_number", i);
      sprintf(query, "SELECT %s FROM customers WHERE username='%s'", column, uname.c_str());
      
      if (!dbase_->dbQuery(query))
      {
        char s[200];
        sprintf(s, "Query to get %s's credit cards failed!\n", player->getUsername());
        Sys_LogError(s);
        return false;
      }
      
      dbFetched = dbase_->dbFetchRow();
      
      if (dbFetched != NULL)
      {
        
        int l = strlen(dbFetched);
        for (int j = 4; j > 0; j--)
        {
          cc.last_4_digits[4-j] = dbFetched[l-j];
        }
        
        cards.push_back(cc);
      }
    }
  }
  
  return true;
};

bool CdbInterface::getShutdown(u_int16_t& shutdown)
{
  char query[MAXQUERYSIZE];
  const char* dbShutdown;
  
  sprintf(query, "SELECT shutdown FROM status");
  
  if (!dbase_->dbQuery(query))
  {
    char s[200];
    sprintf(s, "getShutdown: Query failed!\n");
    Sys_LogError(s);
    // Set shutdown so the server will shut down
    shutdown = 30;
    return false;
  }
  
  dbShutdown = dbase_->dbFetchRow();
  
  if (dbShutdown == NULL)
  {
    return false;
  }
  
  shutdown = atoi(dbShutdown);
  
  return true;
}


bool CdbInterface::getShutdownMsg(string& msg)
{
    char query[MAXQUERYSIZE];
    const char* dbShutdownMsg;

    sprintf(query, "SELECT shutdownmsg FROM status");
    
    if (!dbase_->dbQuery(query))
    {
        char s[200];
        sprintf(s, "getShutdown: Query failed!\n");
        Sys_LogError(s);
        return false;
    }

    dbShutdownMsg = dbase_->dbFetchRow();

    if (dbShutdownMsg == NULL)
    {
        return false;
    }

    msg = dbShutdownMsg;

    return true;
}

bool CdbInterface::getMotdMsg(string& msg)
{
    char query[MAXQUERYSIZE];
    const char* motdmsg = NULL;

    sprintf(query, "SELECT motd FROM status");
    
    if (!dbase_->dbQuery(query))
    {
        char s[200];
        sprintf(s, "getShutdown: Query failed!\n");
        Sys_LogError(s);
        return false;
    }

    motdmsg = dbase_->dbFetchRow();

    if (motdmsg == NULL)
    {
        return false;
    }

    msg = motdmsg;

    return true;
}


bool CdbInterface::getMaxPlayers(long& maxPlayers)
{
  CStrOut query;
  query << "SELECT max_players FROM status";
  
  if (!dbase_->dbQuery(query.str()))
  {
    return false;
  }
  
  const char* fetched = dbase_->dbFetchRow();
  if (!fetched)
  {
    return false;
  }
  
  maxPlayers = atoi(fetched);
  
  return true;
}



bool CdbInterface::addTransaction(CPlayer *player, u_int16_t amount, u_int16_t index, bool debit)
{
    char query[MAXQUERYSIZE];

    string u = sqlfy(player->getUsername());

    if (debit)
    {
    
        memset(query, 0, MAXQUERYSIZE);

        // XXX
        char* ccard = getCardByIndex(player, index);
        if (ccard == NULL)
        {
            char s[200];
            sprintf(s, "addTransaction: Can't get credit card %i for %s\n",
                    index, player->getUsername());
            Sys_LogError(s);
            return false;
        }

        sprintf(query, "INSERT INTO transactions (username, creditcard, type, chips, complete) values(\"%s\", \"%s\", \"debit\", %d, 0)",
            u.c_str(), ccard, amount);

    } else {
        sprintf(query, "INSERT INTO transactions (username, type, chips, complete) values(\"%s\", \"credit\", %d, 0)",
            u.c_str(), amount);
    }
        
    if (!dbase_->dbQuery(query))
    {
        if (DEBUG & DEBUG_DATABASE)
        {
            printf("Query to store %s's transaction failed!\n", player->getUsername());
        }
        return false;
    }

    return true;
};

char* CdbInterface::checkTransaction(CPlayer *player)
{
  char query[MAXQUERYSIZE];
  char results[MAXROWARRAYSIZE][MAXROWSIZE];
  static char retmsg[100];
  
  string uname = sqlfy(player->getUsername());
  
  memset(query, 0, MAXQUERYSIZE);
  memset(results, 0, MAXROWARRAYSIZE * MAXROWSIZE);
  memset(retmsg, 0, 100);
  
  sprintf(query, "SELECT complete, result, reason FROM transactions WHERE username='%s' ORDER BY refnum DESC",
    uname.c_str());
  
  
  if (!dbase_->dbQuery(query))
  {
    if (DEBUG & DEBUG_DATABASE)
    {
      printf("Query to retrieve completed transactions for %s failed!",
        player->getUsername());
    }
    return NULL;
  }
  
  dbase_->dbFetchRowArray(results);
  
  if (strcmp(results[0], "1") == 0)
  {
    if (strcmp(results[1], "approved") == 0)
    {
      sprintf(retmsg, "Transaction Approved!");
    } else {
      sprintf(retmsg, "Transaction Denied (%s)", results[2]);
    }
    
    return retmsg;
  }
  
  return NULL;
};

char* CdbInterface::getCardByIndex(CPlayer *player, u_int16_t index)
{
    char query[MAXQUERYSIZE];
    char column[100];

    memset(query, 0, MAXQUERYSIZE);
    memset(column, 0, 100);

    string u = sqlfy(player->getUsername());

    sprintf(column, "cc%d_number", index+1);
    sprintf(query, "SELECT %s FROM customers WHERE username='%s'", column, u.c_str());

    if (!dbase_->dbQuery(query))
    {
        if (DEBUG & DEBUG_DATABASE)
        {
            printf("Query to get %s's credit card number %d failed!\n", player->getUsername(), index);
        }
        return NULL;
    }

    return dbase_->dbFetchRow();
};


bool CdbInterface::getTournament(int     index,
                                 int& number,
                                 CChips& buyin,
                                 CChips& chips,
                                 CChips& startLo,
                                 u_int32_t& startTime,
                                 int&    maxPlayers,
                                 string& description,
                                 string& script)
{
  CStrOut query;
  const char* result = NULL;

  query << "SELECT number FROM tournament where tournament_index="
        << index;
  
  if (!dbase_->dbQuery(query.str()))
  {
    char s[200];
    sprintf(s, "getTournament: Query failed(1)!\n");
    Sys_LogError(s);
    return false;
  }
  
  result = dbase_->dbFetchRow();
  
  if (result == NULL)
  {
    return false;
  }
  
  number = atoi(result);
  
  // get buyin
  
  query << CStrOut::Clear
        << "SELECT buyin FROM tournament where tournament_index="
        << index;
  
  if (!dbase_->dbQuery(query.str()))
  {
    char s[200];
    sprintf(s, "getTournament: Query failed(5)!\n");
    Sys_LogError(s);
    return false;
  }
  
  result = dbase_->dbFetchRow();
  
  if (result == NULL)
  {
    return false;
  }
  
  buyin = CChips(atof(result));
  
  // get start chips
  
  query << CStrOut::Clear
        << "SELECT start_chips FROM tournament where tournament_index="
        << index;
  
  if (!dbase_->dbQuery(query.str()))
  {
    char s[200];
    sprintf(s, "getTournament: Query failed(5,5)!\n");
    Sys_LogError(s);
    return false;
  }
  
  result = dbase_->dbFetchRow();
  
  if (result == NULL)
  {
    return false;
  }
  
  chips = CChips(atof(result));
  
  // get start low limit
  
  query << CStrOut::Clear
        << "SELECT start_lo FROM tournament where tournament_index="
        << index;
  
  if (!dbase_->dbQuery(query.str()))
  {
    char s[200];
    sprintf(s, "getTournament: Query failed(5,6)!\n");
    Sys_LogError(s);
    return false;
  }
  
  result = dbase_->dbFetchRow();
  
  if (result == NULL)
  {
    return false;
  }
  
  startLo = CChips(atof(result));
  
  
  // get start time

  query << CStrOut::Clear  
        << "SELECT start_time FROM tournament where tournament_index="
        << index;
  
  if (!dbase_->dbQuery(query.str()))
  {
    char s[200];
    sprintf(s, "getTournament: Query failed(2)!\n");
    Sys_LogError(s);
    return false;
  }
  
  result = dbase_->dbFetchRow();
  
  if (result == NULL)
  {
    return false;
  }
  
  startTime = atol(result);

  // get max number of players

  query << CStrOut::Clear
        << "SELECT max_players FROM tournament where tournament_index="
        << index;
  
  if (!dbase_->dbQuery(query.str()))
  {
    char s[200];
    sprintf(s, "getTournament: Query failed(max_players)!\n");
    Sys_LogError(s);
    return false;
  }
  
  result = dbase_->dbFetchRow();
  
  if (result == NULL)
  {
    // use whatever is default
  }
  else
  {
    maxPlayers = atoi(result);
  }
  
  // get description
  query << CStrOut::Clear  
        << "SELECT description FROM tournament where tournament_index="
        << index;
  
  if (!dbase_->dbQuery(query.str()))
  {
    char s[200];
    sprintf(s, "getTournament: Query failed(3)!\n");
    Sys_LogError(s);
    return false;
  }
  
  result = dbase_->dbFetchRow();
  
  if (result == NULL)
  {
    return false;
  }
  
  description = result;
  
  // get script
  
  query << CStrOut::Clear
        << "SELECT script FROM tournament where tournament_index="
        << index;
  
  if (!dbase_->dbQuery(query.str()))
  {
    char s[200];
    sprintf(s, "getTournament: Query failed(4)!\n");
    Sys_LogError(s);
    return false;
  }
  
  result = dbase_->dbFetchRow();
  
  if (result == NULL)
  {
    return false;
  }
  
  script = result;
  
  return true;
}


//
// MFUNCTION: CdbInterface::getSatellite
//
// PURPOSE:   Increments current satellite number and returns
//            parameters for a new satellite tournament.
//            
//
bool CdbInterface::getSatellite(int        index,
                                int&       number,
                                CChips&    buyin,
                                CChips&    chips,
                                CChips&    startLo,
                                int&       numPlayers,
                                string&    description,
                                string&    script)
{
    CStrOut query;
    const char* result = NULL;


    //
    // Make a satellite that has number 1 greater than
    // the last satellite.
    //

    query << "select max(number) from satellite";
    if (!dbase_->dbQuery(query.str()))
    {
        char s[200];
        sprintf(s, "getSatellite: Query failed(0)!\n");
        Sys_LogError(s);
        return false;
    }

    result = dbase_->dbFetchRow();
    if (!result)
    {
        return false;
    }

    int max_satellite = atoi(result);


    query << CStrOut::Clear << "select max(number) from tournament";
    if (!dbase_->dbQuery(query.str()))
    {
        char s[200];
        sprintf(s, "getSatellite: Query failed(0.5)!\n");
        Sys_LogError(s);
        return false;
    }

    result = dbase_->dbFetchRow();
    if (!result)
    {
        return false;
    }

    number = atoi(result);

    number = max(number, max_satellite);
    number++;


    query << CStrOut::Clear
          << "UPDATE satellite SET number="  << number << " where tournament_index="
          << index;
    
    if (!dbase_->dbQuery(query.str()))
    {
        char s[200];
        sprintf(s, "getSatellite: Query failed(1)!\n");
        Sys_LogError(s);
        return false;
    }

    query << CStrOut::Clear
          << "SELECT number FROM satellite where tournament_index="
          << index;
    
    if (!dbase_->dbQuery(query.str()))
    {
        char s[200];
        sprintf(s, "getSatellite: Query failed(2)!\n");
        Sys_LogError(s);
        return false;
    }

    result = dbase_->dbFetchRow();

    if (result == NULL)
    {
        return false;
    }

    number = atoi(result);

    // get buyin

    query << CStrOut::Clear
          << "SELECT buyin FROM satellite where tournament_index="
          << index;
    
    if (!dbase_->dbQuery(query.str()))
    {
        char s[200];
        sprintf(s, "getSatellite: Query failed(3)!\n");
        Sys_LogError(s);
        return false;
    }

    result = dbase_->dbFetchRow();

    if (result == NULL)
    {
        return false;
    }

    buyin = atof(result);

    // get start chips

    query << CStrOut::Clear
          << "SELECT start_chips FROM satellite where tournament_index="
          << index;
    
    if (!dbase_->dbQuery(query.str()))
    {
        char s[200];
        sprintf(s, "getSatellite: Query failed(4)!\n");
        Sys_LogError(s);
        return false;
    }

    result = dbase_->dbFetchRow();

    if (result == NULL)
    {
        return false;
    }

    chips = atof(result);

    // get start low limit

    query << CStrOut::Clear
          << "SELECT start_lo FROM satellite where tournament_index="
          << index;
    
    if (!dbase_->dbQuery(query.str()))
    {
        char s[200];
        sprintf(s, "getSatellite: Query failed(5)!\n");
        Sys_LogError(s);
        return false;
    }

    result = dbase_->dbFetchRow();

    if (result == NULL)
    {
        return false;
    }

    startLo = atof(result);

    // get number of players
    query << CStrOut::Clear
          << "SELECT num_players FROM satellite where tournament_index="
          << index;
    
    if (!dbase_->dbQuery(query.str()))
    {
        char s[200];
        sprintf(s, "getSatellite: Query failed(5,5)!\n");
        Sys_LogError(s);
        return false;
    }

    result = dbase_->dbFetchRow();

    if (result == NULL)
    {
        return false;
    }

    numPlayers = atoi(result);

    // get description
    query << CStrOut::Clear
          << "SELECT description FROM satellite where tournament_index="
          << index;
    
    if (!dbase_->dbQuery(query.str()))
    {
        char s[200];
        sprintf(s, "getSatellite: Query failed(6)!\n");
        Sys_LogError(s);
        return false;
    }

    result = dbase_->dbFetchRow();

    if (result == NULL)
    {
        return false;
    }

    description = result;

    // get script
    query << CStrOut::Clear
          << "SELECT script FROM satellite where tournament_index="
          << index;
    
    if (!dbase_->dbQuery(query.str()))
    {
        char s[200];
        sprintf(s, "getSatellite: Query failed(7)!\n");
        Sys_LogError(s);
        return false;
    }

    result = dbase_->dbFetchRow();

    if (result == NULL)
    {
        return false;
    }

    script = result;

    return true;
}



struct StrmFreezer
{
  StrmFreezer(ostrstream& o)
    : os_(o)
  {
    str_ = os_.str();
  }
  ~StrmFreezer()
  {
    os_.rdbuf()->freeze(0);
    str_ = NULL;
  }
  
  const char* c_str() const
  {
    return str_;
  }
  char* str() const
  {
    return const_cast<char*>(str_);
  }
  
  ostrstream& os_;
  const char* str_;
};

bool CdbInterface::logTournamentEvent(int tournament,
                                      int eventId,
                                      long eventTime,
                                      const char* player,
                                      const char* extra)
{
  WriteLockSQLTable sqlLock(dbase_, TOURNAMENTTABLE);
  if (!sqlLock.success_)
    return false;
  
  /*
    QUERY:
    
      INSERT INTO tournamentlog (number, event, time, player, extra) values
      (<number>, <eventid>, <eventtime>, <player>, <extra>);
  */
  
  ostrstream query;
  string n;
  if (player && *player != '\0')
    n = sqlfy(player);
  
  if (eventTime == 0)
    eventTime = time(NULL);
  
  query << "INSERT INTO " << TOURNAMENTTABLE <<
    " (number, event, time, player, extra) values (";
  
  query << tournament << ", ";
  query << eventId << ", ";
  query << eventTime <<", ";
  query << "'";
  if (n.size() > 0)
    query << n.c_str();
  query << "', ";
  query << "'";
  if (extra && *extra != '\0')
    query << extra;
  query << "');";
  query << '\0';
  
  /*
  sprintf(query, "INSERT INTO %s (number, event, time, player, extra) values(%d, \"\")", 
  TOURNAMENTTABLE,
  tournament, eventId, eventTime,
  player ? player : "",
  extra ? extra : "");
  */
  
  StrmFreezer f(query);
  if (!dbase_->dbQuery(f.str())) 
  {
    Sys_LogError("Tournament log query failed:");
    Sys_LogError(f.c_str());
    return false;
  }
  
  return true;
}


bool CdbInterface::buyin(CPlayer*  player, const CChips& buyin)
{
  // XXX No cent buyins for now
  CChips::chip_amount_t buyinChips = buyin.getDollars();
  
  // freeroll tournaments have 0 chip buyin
  if (buyinChips == 0) 
    return true;
  
  WriteLockSQLTable sqlLock(dbase_, "pokeruser");
  if (!sqlLock.success_)
    return false;
  
  char  query[MAXQUERYSIZE];
  char* dbFetched = NULL;
  
  string name = sqlfy(player->getUsername());
  
  sprintf(query, "SELECT chips FROM pokeruser WHERE username='%s'", name.c_str());
  
  if (!dbase_->dbQuery(query))
  {
    if (DEBUG & DEBUG_DATABASE)
    {
      printf("Query to get %s's chips failed!\n", player->getUsername());
    }
    
    return false;
  }
  
  dbFetched = dbase_->dbFetchRow();
  
  if (dbFetched == NULL)
  {
    if (DEBUG & DEBUG_DATABASE)
    {
      printf("User %s does not exist in database!\n", player->getUsername());
    }
    
    return false;
  }
  
  CChips::chip_amount_t chips = (CChips::chip_amount_t)atoi(dbFetched);
  
  if (chips < buyinChips)
    return false;
  
  chips = chips - buyinChips;
  
  sprintf(query, "UPDATE pokeruser set chips=%d WHERE username='%s'", chips, name.c_str());
  
  if (!dbase_->dbQuery(query))
  {
    if (DEBUG & DEBUG_DATABASE)
    {
      printf("Query to get %s's chips failed!\n", player->getUsername());
    }
    
    return false;
  }
  
  sprintf(query, "Player %s tourney-buyin %d, %d",
          name.c_str(), buyinChips, chips);
  Sys_LogError(query);
  
  return true;
}


// Update parameters in database so as to schedule a new tournament
// to start at designated time.
bool CdbInterface::scheduleNewTournament(u_int32_t startTime,
                                         int tournamentIndex)
{
  CStrOut query;
  
  // Get current max tournament number from database
  query << "select max(number) from satellite";
  if (!dbase_->dbQuery(query.str()))
  {
    Sys_LogError("Unable to update tournament(0)\n");
    return false;
  }
  
  const char* fetched = dbase_->dbFetchRow();
  int satelliteNumber = 0;
  
  if (fetched != NULL)
  {
    satelliteNumber = atoi(fetched);
  }

  query << CStrOut::Clear << "select max(number) from tournament";
  if (!dbase_->dbQuery(query.str()))
  {
    Sys_LogError("Unable to update tournament(1)\n");
    return false;
  }

  fetched = dbase_->dbFetchRow();
  int number = 0;

  if (fetched != NULL)
  {
    number = atoi(fetched);
  }

  number = max(satelliteNumber, number);
  number++;


  query << CStrOut::Clear
        << "UPDATE tournament SET number=" << number
        << ", start_time=" << startTime
        << " where tournament_index=" << tournamentIndex;
    
  if (!dbase_->dbQuery(query.str()))
  {
    Sys_LogError("Unable to update tournament(1)\n");
    return false;
  }

  return true;
}


bool CdbInterface::checkTournament(int tournament)
{
    ReadLockSQLTable sqlLock(dbase_, TOURNAMENTTABLE);
    if (!sqlLock.success_)
        return false;

    /*
        QUERY:

        SELECT COUNT(*) FROM tournamentlog WHERE number=<number> AND
                                                 eventid=<TE_PlayerJoin>;
    */

    ostrstream query;
    query << "SELECT COUNT(*) FROM " << TOURNAMENTTABLE <<
             " WHERE number=" << tournament << " AND event=" <<
             (int)TE_PlayerJoin << ';' << '\0';

    {
        StrmFreezer f(query);
        if (!dbase_->dbQuery(f.str())) 
        {
            Sys_LogError(f.c_str());
            return false;
        }
    }

    const char* fetched = dbase_->dbFetchRow();
    int count1 = 0;

    if (fetched != NULL)
    {
        count1 = atoi(fetched);
    }    

    /*
        QUERY:

        SELECT COUNT(*) FROM tournamentlog WHERE number=<number> AND
                                                 eventid=<TE_PlayerLeave>;
    */

    query.clear();

    query << "SELECT COUNT(*) FROM " << TOURNAMENTTABLE <<
             " WHERE number=" << tournament << " AND event=" <<
             (int)TE_PlayerLeave << ';' << '\0';

    {
        StrmFreezer f(query);
        if (!dbase_->dbQuery(f.str())) 
        {
            Sys_LogError(f.c_str());
            return false;
        }
    }

    fetched = dbase_->dbFetchRow();
    int count2 = 0;

    if (fetched != NULL)
    {
        count2 = atoi(fetched);
    }    

    printf("Tournament players: joined %i left %i\n", count1, count2);

    return count1 == count2;
}


bool CdbInterface::countTournamentPlayers(int tournament,
                                          int& joined,
                                          int& left)
{
    ReadLockSQLTable sqlLock(dbase_, TOURNAMENTTABLE);
    if (!sqlLock.success_)
        return false;

    /*
        QUERY:

        SELECT COUNT(*) FROM tournamentlog WHERE number=<number> AND
                                                 eventid=<event>;
    */

    ostrstream query;
    query << "SELECT COUNT(*) FROM " << TOURNAMENTTABLE <<
             " WHERE number=" << tournament << " AND event=" <<
             (int)TE_PlayerJoin << ';' << '\0';

    {
        StrmFreezer f(query);
        if (!dbase_->dbQuery(f.str())) 
        {
            Sys_LogError(f.c_str());
            return false;
        }
    }

    const char* fetched = dbase_->dbFetchRow();

    if (fetched != NULL)
    {
        joined = atoi(fetched);
    }
    else
        return false;

    query.clear();

    query << "SELECT COUNT(*) FROM " << TOURNAMENTTABLE <<
             " WHERE number=" << tournament << " AND event=" <<
             (int)TE_PlayerLeave << ';' << '\0';

    {
        StrmFreezer f(query);
        if (!dbase_->dbQuery(f.str())) 
        {
            Sys_LogError(f.c_str());
            return false;
        }
    }

    fetched = dbase_->dbFetchRow();
    if (fetched != NULL)
    {
        left = atoi(fetched);
    }    
    else
        return false;

    return true;
}


bool CdbInterface::setTournamentFormat(const string& s)
{
    char query[MAXQUERYSIZE];
    memset(query, 0, MAXQUERYSIZE);

    if (2 * s.size() + 1 >= (MAXQUERYSIZE - 50))
    {
        Sys_LogError("CdbInterface::setTournamentFormat buffer size too small!");
        return false;
    }

    char* fmt = new char[2 * s.size() + 1];
    if (!fmt)
    {
        Sys_LogError("CdbInterface::setTournamentFormat out of memory!");
        return false;
    }

    memset(fmt, 0, 2 * s.size() + 1);
    mysql_escape_string(fmt, s.c_str(), s.size());

    sprintf(query, "UPDATE tournament SET description='%s';",
            fmt);

    delete [] fmt;
    fmt = NULL;

    if (!dbase_->dbQuery(query))
    {
        Sys_LogError("Query to set tournament format failed.");
        return false;
    }

    return true;
}

bool CdbInterface::setTournamentRules(const string& s)
{
    return true;
}


// The player needs to have a credit card and
// to be on the mailing list in order to play
bool CdbInterface::checkTournament(const CPlayer& p)
{
    // XXX: for now, everyone is allowed to join
    return true;

#if 0

  // Check the VIP flag
    char query[MAXQUERYSIZE];
    char* dbFetched = NULL;
    memset(query, 0x0, MAXQUERYSIZE);

    string name = sqlfy(p.getUsername());

    sprintf(query, "SELECT VIP FROM customers WHERE username='%s'",
            name.c_str());
    
    if (!dbase_->dbQuery(query))
    {
        Sys_LogError("SELECT VIP query failed.\n");
        return false;
    }

    dbFetched = dbase_->dbFetchRow();

    if (dbFetched == NULL)
    {
        return false;
    }

    int vip = atoi(dbFetched);
    return vip != 0;
#endif
}

// This mfunction creates 'numPlayers' player entries
// with username and password 'bot_X'.
bool CdbInterface::createTestPlayers(int numPlayers)
{
    bool rc = true;

    for (int i = 0; i < numPlayers; i++)
    {
        char buf[17];
        sprintf(buf, "bot_%i", i + 1);

        // First check that the player is not already there!
        char query[MAXQUERYSIZE];
        sprintf(query, "SELECT username FROM pokeruser where username='%s';",
                buf);

        if (!dbase_->dbQuery(query))
        {
            Sys_LogError("Query to create test player failed(0).");
            return false;
        }

        const char* fetched = dbase_->dbFetchRow();
        if (fetched == NULL)
        {   // Okay now create the player
            MD5 context;
            context.update((unsigned char*)buf, strlen(buf));
            context.finalize();

            sprintf(query, "INSERT INTO customers (username, password, cc1_type, cc1_number, spam) VALUES ('%s', '%s', 'asiV', '1', 1);",
                    buf, context.hex_digest());
    
            if (!dbase_->dbQuery(query))
            {
                Sys_LogError("Query to create test player failed(1).");
                return false;
            }        

            sprintf(query, "INSERT INTO pokeruser (username, password) VALUES ('%s', '%s');",
                    buf, context.hex_digest());
    
            if (!dbase_->dbQuery(query))
            {
                Sys_LogError("Query to create test player failed(2).");
                return false;
            }        
        }
    }

    return true;
}


bool CdbInterface::getTestState(int& state)
{
  CStrOut query;
  query << "SELECT state FROM test_state";
  
  if (!dbase_->dbQuery(query.str()))
  {
    return false;
  }
  
  const char* fetched = dbase_->dbFetchRow();
  if (!fetched)
  {
    return false;
  }

  state = atoi(fetched);
  
  return true;
}


bool CdbInterface::getCreditCardData(const char* username,
                                     list<CreditCardData>& creditCards)
{
  if (!username || *username == '\0')
    return true;

  char results[MAXROWARRAYSIZE][MAXROWSIZE];
  string qusername = quote(username);

  for (int i = 1; i < 5; i++)
  {

    memset(results, 0, sizeof(results));

    /*
      QUERY: SELECT cc1_type, cc1_number, cc1_expmo, cc1_expyr
              FROM customers WHERE username=<username>;
    */

    ostrstream query;

    query << "SELECT cc" << i << "_type, cc" << i << "_number, cc"
          << i << "_expmo, cc" << i << "_expyr FROM customers WHERE username="
          << qusername;
    query << ends;

    StrmFreezer f(query);
    if (!dbase_->dbQuery(f.str())) 
    {
      Sys_LogError("Query failed:");
      Sys_LogError(f.c_str());
      return false;
    }

    dbase_->dbFetchRowArray(results);

    if (results[1] == NULL ||
        *(results[1]) == '\0' ||
        *(results[1]) == ' ')
      break;

    CreditCardData ccdata(results[0],
                          results[1],
                          results[2],
                          results[3]);

    creditCards.push_back(ccdata);
  }

  return true;
}

bool CdbInterface::saveCreditCardData(const char* username,
                                      const list<CreditCardData>& ccards)
{
  if (!username || *username == '\0')
    return true;

  string qusername = quote(username);

  int index = 1;
  for (list<CreditCardData>::const_iterator it = ccards.begin(),
                                            end = ccards.end();
       it != end && index < 5;
       ++it, ++index)
  {
    /*
      QUERY: UPDATE customers SET
               cc1_type=<>, cc1_number=<>, cc1_expmo=<>, cc1_expyr=<>
              WHERE username=<username>;
    */

    const CreditCardData& ccard = (*it);

    ostrstream query;

    string qcctype = quote(ccard.cctype_.c_str());
    string qccnumber = quote(ccard.ccnumber_.c_str());

    query << "UPDATE customers SET " <<
             "cc"   << index << "_type = " << qcctype <<
             ", cc" << index << "_number = " << qccnumber <<
             ", cc" << index << "_expmo = " << ccard.ccexpmo_ <<
             ", cc" << index << "_expyr = " << ccard.ccexpyr_ <<
             " WHERE username=" << qusername;
    query << ends;

    StrmFreezer f(query);
    //    printf("%s\n", f.str());
    if (!dbase_->dbQuery(f.str())) 
    {
      Sys_LogError("Query failed:");
      Sys_LogError(f.c_str());
      return false;
    }
  }
  
  // Zero out extra credit cards - this allows players to
  // remove credit card data from their accounts.
  for (; index < 5; index++)
  {
    /*
      QUERY: UPDATE customers SET
               cc1_type=NULL, cc1_number=NULL, cc1_expmo=NULL, cc1_expyr=NULL
              WHERE username=<username>;
    */

    ostrstream query;

    query << "UPDATE customers SET " <<
             "cc"   << index << "_type = NULL" <<
             ", cc" << index << "_number = NULL" <<
             ", cc" << index << "_expmo = NULL" <<
             ", cc" << index << "_expyr = NULL" <<
             " WHERE username=" << qusername;
    query << ends;

    StrmFreezer f(query);
    //    printf("%s\n", f.str());
    if (!dbase_->dbQuery(f.str())) 
    {
      Sys_LogError("Query failed:");
      Sys_LogError(f.c_str());
      return false;
    }
  }


  return true;
}


// Return true if player is in the chat ban list,
// false otherwise.
bool CdbInterface::checkChatBan(const char* username)
{
  string s = quote(username);

  /*
      QUERY:
      SELECT username FROM chat_ban WHERE username='username';
  */

  ostrstream query;
  query << "SELECT username FROM chat_ban WHERE username="<< s.c_str() << ends;

  StrmFreezer f(query);
  if (!dbase_->dbQuery(f.str())) 
    return false;

  const char* fetched = dbase_->dbFetchRow();
  if (fetched == NULL)
    return false;

  // username is in the ban list
  return true;
}