#ifdef _WIN32
#include <strstrea.h>
#else
#include <strstream.h>
#endif
#include "dbinterface.h"
#include "tournament/tournamentevent.h"
#include <string>


// NOTE NOTE NOTE:
// It is important that the CdbInterface mfunctions
// do not call into anything that might lock another
// mutex (e.g., CTable mfunctions) because that could
// lead into a dead-lock.

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

        if (!success_)
            Sys_LogError(query);
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

        if (!success_)
            Sys_LogError(query);
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



CdbInterface::CdbInterface(CdbHandler* dbase)
  :
  dbase_(dbase)
{}


bool CdbInterface::authenticate(const char* username, const char* password)
{
    SingleLock l(&dbMutex_);
    if (!l.lock())
        return false;

    ReadLockSQLTable sqlLock(dbase_, "pokeruser");
    if (!sqlLock.success_)
        return false;

    char query[MAXQUERYSIZE];
    char* dbPassword = NULL;
    MD5 context;

    string u = sqlfy(username);

    context.update((unsigned char *)password, (int)strlen(password));
    context.finalize();


    memset(query, 0x0, MAXQUERYSIZE);

    sprintf(query, "SELECT password FROM pokeruser WHERE username='%s'", u.c_str());
    
    if (!dbase_->dbQuery(query))
    {
        if (DEBUG & DEBUG_DATABASE)
        {
            printf("Query to authenticate %s failed!\n", username);
            printf("Reason: %s\n", mysql_error(dbase_->mySQLQuery_));
        }
        return false;
    }

    dbPassword = dbase_->dbFetchRow();

    if (dbPassword == NULL)
    {
        char s[200];
        sprintf(s, "CTable::tableLogin: User %s does not exist in database!", username);
        Sys_LogError(s);
        return false;
    }
	//MP
    //else if (strcmp(dbPassword, context.hex_digest()))
	else if (strcmp(dbPassword, password))
    {
        char s[200];
        sprintf(s, "CTable::tableLogin: wrong password %s for user %s.", password, username);
        Sys_LogError(s);
        return false;
    }

    return true;
};

bool CdbInterface::getCity(const char* username, string& city)
{
    SingleLock l(&dbMutex_);
    if (!l.lock())
        return false;

    char query[MAXQUERYSIZE];
    const char* dbCity = NULL;

    string u = sqlfy(username);

    sprintf(query, "SELECT city FROM customers WHERE username='%s'", u.c_str());
    
    if (!dbase_->dbQuery(query))
    {
        if (DEBUG & DEBUG_DATABASE)
        {
            printf("Query to get player's city failed: %s!\n", username);
        }
        return false;
    }

    bool rc = false;
    dbCity = dbase_->dbFetchRow();

    if (dbCity != NULL)
    {
        city = dbCity;
        rc = true;
    }
    else
    {
        rc = false;
    }

    return rc;
};


bool CdbInterface::getChips(CPlayer* player, CChips& chips)
{
  SingleLock l(&dbMutex_);
  if (!l.lock())
    return false;
  
  ReadLockSQLTable sqlLock(dbase_, "pokeruser");
  if (!sqlLock.success_)
    return false;
  
  char query[MAXQUERYSIZE];
  char *dbFetched = NULL;
  
  memset(query, 0x0, MAXQUERYSIZE);
  
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
    
    return (u_int16_t)0;
    
  }
  
  double c = atof(dbFetched);
  chips = CChips(c);
  return true;
};

bool CdbInterface::storeChips(CPlayer* player, const CChips& chips)
{
    SingleLock l(&dbMutex_);
    if (!l.lock())
        return false;

    WriteLockSQLTable sqlLock(dbase_, "pokeruser");
    if (!sqlLock.success_)
        return false;

    char query[MAXQUERYSIZE];
    memset(query, 0x0, MAXQUERYSIZE);
    //char buffer[20];

    string name = sqlfy(player->getUsername());
    
    //itoa(chips, buffer, 10);
    sprintf(query, "UPDATE pokeruser SET chips=%f where username='%s'",
            chips.asDouble(), name.c_str());
    
    if (!dbase_->dbQuery(query))
    {
        if (DEBUG & DEBUG_DATABASE)
        {
            printf("Query to store %s's chips failed!\n", player->getUsername());
        }

        return false;
    }

    return true;
};

// Initialize log buffer to empty.
bool CdbInterface::openLog()
{
    logBuf_ = "\"";
    return true;
}

bool CdbInterface::putHandLogRequest(CPlayer *player, const char *email, int numhands)
{
    SingleLock l(&dbMutex_);
    if (!l.lock())
        return false;

    // XXX
    string uname = sqlfy(player->getUsername());

    char query[MAXQUERYSIZE];
    char fields[MAXROWARRAYSIZE][MAXROWSIZE];
    int i;

    memset(query, 0, MAXQUERYSIZE);
    memset(fields, 0, MAXROWARRAYSIZE * MAXROWSIZE);

    if (numhands == 5)
    {
        // XXX
        sprintf(query, "SELECT game1, game2, game3, game4, game5 FROM pokeruser WHERE username=\"%s\"",
                uname.c_str());
    } else {
        // XXX
        sprintf(query, "SELECT game1 FROM pokeruser WHERE username=\"%s\"",
                uname.c_str());
    }

    if (!dbase_->dbQuery(query))
    {
        if (DEBUG & DEBUG_DATABASE)
        {
            printf("Query to store chat message failed!\n");
        }
        return false;
    }


    memset(query, 0, MAXQUERYSIZE);

    if (numhands == 5)
    {
        for (i = 0; i < 5; i++)
            if (fields[i])
                strcpy(fields[i], "NULL");

        dbase_->dbFetchRowArray(fields);
        // XXX
        sprintf(query, "INSERT INTO logrequests (username, email, game1, game2, game3, game4, game5) values(\"%s\", \"%s\", %s, %s, %s, %s, %s)",
                uname.c_str(), email, fields[0], fields[1], fields[2], fields[3], fields[4]);

    } else {
        const char* p = dbase_->dbFetchRow();
        if (p == NULL)
            return false;

        strncpy(fields[0], p, MAXROWSIZE);
        // XXX
        sprintf(query, "INSERT INTO logrequests (username, email, game1, game2, game3, game4, game5) values(\"%s\", \"%s\", %s, NULL, NULL, NULL, NULL)",
                uname.c_str(), email, fields[0]);
    }

    if (!dbase_->dbQuery(query))
    {
        if (DEBUG & DEBUG_DATABASE)
        {
            printf("Query to store chat message failed!\n");
        }
        return false;
    }

    return true;

};


// Flush current log buffer to database.
bool CdbInterface::flushLog(u_int32_t gameNumber)
{
    // XXX tournament (testing!)
#ifdef _WIN32
//    return true;
#endif

    if (logBuf_.size() > (MAXQUERYSIZE / 2 - 1))
    {
        // crop the message to prevent overflow        
        logBuf_ = logBuf_.substr(0, (MAXQUERYSIZE / 2 - 1));
    }

    for (int i = 1; i < logBuf_.size(); i++)
    {
        if (logBuf_[i] == '"')
            logBuf_[i] = ' ';  // quick hack to change " to space so that nobody
    }                           // can escape out of the mysql entry.. huge
                                // security hole without this.

    // Append the last double quote
    logBuf_ += "\"";
    
    SingleLock l(&dbMutex_);
    if (!l.lock())
        return false;

    // Update is atomic, no locking needed

    char getquery[MAXQUERYSIZE] = { '\0' };
    char setquery[MAXQUERYSIZE] = { '\0' };
    char text[MAXQUERYSIZE/2] = { '\0' };
    char merged[MAXQUERYSIZE] = { '\0' };
    char *dbFetched = NULL;

    sprintf(setquery, "UPDATE %s set text=%s where gamenumber=%d", GAMETABLE,
            logBuf_.c_str(), gameNumber);
    
    if (!dbase_->dbQuery(setquery)) // store chat message in game log
    {
        if (DEBUG & DEBUG_DATABASE)
        {
            printf("Query to store chat message failed!\n");
        }
        return false;
    }
    
    return true;
}

// Append message to log buffer. Log entries are separated
// by a newline.
bool CdbInterface::setLog(u_int32_t gameNumber, const char* charLog)
{
  const string nl("\n");
  logBuf_ += (charLog + nl);
  return true;
};

bool CdbInterface::createGameEntry(u_int32_t gameNumber)
{
    SingleLock l(&dbMutex_);
    if (!l.lock())
        return false;

    WriteLockSQLTable sqlLock(dbase_, GAMETABLE);
    if (!sqlLock.success_)
        return false;

    char query[MAXQUERYSIZE] = { '\0' };

    sprintf(query, "INSERT INTO %s (gamenumber, text) values(%d, \"\")", 
            GAMETABLE, gameNumber);

    if (!dbase_->dbQuery(query)) // create new table
    {
        if (DEBUG & DEBUG_DATABASE)
        {
            printf("Query to create TABLE GAME%d failed!\n", gameNumber);
        }

        return false;
    }

    return true;

};


u_int32_t CdbInterface::getGameNum(void)
{
    SingleLock l(&dbMutex_);
    if (!l.lock())
        return 0;

    WriteLockSQLTable sqlLock(dbase_, "house");
    if (!sqlLock.success_)
        return 0;

    // XXX: this order of the queries seems to 
    // work around the mysql crash problems (hopefully
    // a win32-only issue)!
    char* query1 = "SELECT total_games FROM house";
    char query2[MAXQUERYSIZE];
    char* dbFetched = NULL;
    u_int32_t gameNumber = 0;
    
    if (!dbase_->dbQuery(query1))   // get total_games
    {
        Sys_LogError("Query to get gamenumber failed!\n");

        return (u_int32_t)0;
    }

    dbFetched = dbase_->dbFetchRow();

    if (dbFetched == NULL)
    {
        if (DEBUG & DEBUG_DATABASE)
        {
            printf("gamenumber or possible house table, nonexistant in database!");
        }

        //return (u_int32_t)0;
		sprintf(query1,"INSERT into house (total_games) VALUES(%d)", gameNumber);
		if (!dbase_->dbQuery(query1)) {
			printf("failure inserting gamenumber");
			return (u_int32_t)0;
		}
    }


    gameNumber = (u_int32_t)atoi(dbFetched);

    gameNumber++;

    sprintf(query2, "UPDATE house SET total_games = %d", gameNumber);

    if (!dbase_->dbQuery(query2))   // select total_games from house
    {
        Sys_LogError("Query to set gamenumber failed!\n");

        return (u_int32_t)0;
    }

/*
    char query2[MAXQUERYSIZE] = "UPDATE house SET total_games = total_games+1";
    char query3[MAXQUERYSIZE] = "SELECT total_games FROM house";
    char *dbFetched;
    u_int32_t gameNumber = 0;
    
    if (!dbase_->dbQuery(query2))   // update total_games++;
    {
        Sys_LogError("Query to get gamenumber failed!\n");

        return (u_int32_t)0;
    }

    if (!dbase_->dbQuery(query3))   // select total_games from house
    {
        Sys_LogError("Query to get gamenumber failed!\n");

        return (u_int32_t)0;
    }

    dbFetched = dbase_->dbFetchRow();

    if (dbFetched == NULL)
    {
        if (DEBUG & DEBUG_DATABASE)
        {
            printf("gamenumber or possible house table, nonexistant in database!");
        }

        return (u_int32_t)0;
    }

    gameNumber = (u_int32_t)atoi(dbFetched);
*/
    return gameNumber;
};


// Does an atomic buy-in, i.e., subtracts 'chipsRequested'
// chips from user's account on database.
// The amount of chips successfully bought-in is returned
// in 'chipsBoughtIn'.
bool CdbInterface::buyinChips(CPlayer* player,
                              CChips chipsRequested,
                              CChips& chipsBoughtIn)
{
    chipsBoughtIn = 0;

    SingleLock l(&dbMutex_);
    l.lock();

    // Lock the table so no-one can change the chips in
    // between the read and write here.
    WriteLockSQLTable sqlLock(dbase_, "pokeruser");

    string name = sqlfy(player->getUsername());
    char *dbFetched = NULL;
    char query[MAXQUERYSIZE];

    sprintf(query, "SELECT chips FROM pokeruser WHERE username='%s'", name.c_str());
        
    if (!dbase_->dbQuery(query))
    {
        char s[200];
        sprintf(s, "buyinChips: Query to get %s's chips failed!\n", player->getUsername());
        Sys_LogError(s);
        return false;
    }

    dbFetched = dbase_->dbFetchRow();

    if (dbFetched == NULL)
    {
        char s[200];
        sprintf(s, "buyinChips: User %s does not exist in database!\n", player->getUsername());
        Sys_LogError(s);
        return false;
    }

    CChips dbChips(atof(dbFetched));
    //long dbChips = atol(dbFetched);

    if (dbChips < chipsRequested)
    {
        // Player is asking for more chips than he has.
        // This can happen if he's logging into multiple
        // tables at the same time. It is not a problem
        // because we handle it here.
        chipsRequested = dbChips;
    }

    sprintf(query, "UPDATE pokeruser SET chips=%9.2f where username='%s'",
            (dbChips - chipsRequested).asDouble(), name.c_str());
    
    bool rc = dbase_->dbQuery(query);
    if (!rc)
    {
        char s[200];
        sprintf(s, "Query to save %s's chips (%9.2f) failed!\n",
                player->getUsername(), (dbChips - chipsRequested).asDouble());
        Sys_LogError(s);
    }
    else
    {   // Success, return number of chips bought in
        chipsBoughtIn = chipsRequested;
    }

    Sys_LogChipBuyin(player->getUsername(),
                     (dbChips - chipsRequested).asDouble(),
                     chipsBoughtIn.asDouble());

    return rc;
}


bool CdbInterface::saveChips(CPlayer* player, const CChips& chips)
{
    SingleLock l(&dbMutex_);
    l.lock();

// XXX
// Is the write lock really needed? I read from mysql docs that
// usually you don't need to lock tables explicitely.

    WriteLockSQLTable sqlLock(dbase_, "pokeruser");
    if (!sqlLock.success_)
        return false;

    string name = sqlfy(player->getUsername());

    char query[MAXQUERYSIZE];
    // Atomically increment player's chips in the database by 'chips'.
    sprintf(query, "UPDATE pokeruser SET chips=chips+%f where username='%s'",
            chips.asDouble(), name.c_str());
    
    if (!dbase_->dbQuery(query))
    {
        char s[200];
        sprintf(s, "Query to save %s's chips (%d) failed!\n", player->getUsername(), chips);
        Sys_LogError(s);
        return false;
    }

    sprintf(query, "select chips from pokeruser where username='%s'",
            name.c_str());
    if (!dbase_->dbQuery(query))
    {
        char s[200];
        sprintf(s, "saveChips: Query to get %s's chips failed!\n", player->getUsername());
        Sys_LogError(s);
    }

    const char* dbFetched = dbase_->dbFetchRow();

    CChips dbChips;
    if (dbFetched == NULL)
    {
        char s[200];
        sprintf(s, "buyinChips: User %s does not exist in database!\n", player->getUsername());
        Sys_LogError(s);
    }
    else
    {
      dbChips = atof(dbFetched);
    }

    Sys_LogChipSave(player->getUsername(),
                    dbChips.asDouble(), chips.asDouble());

    return true;
}

bool CdbInterface::saveRake(const CChips& rake)
{
    SingleLock l(&dbMutex_);
    l.lock();
    
    char query[MAXQUERYSIZE];
    sprintf(query, "UPDATE house SET cash=cash+%9.2f", rake.asDouble());
   
    if (!dbase_->dbQuery(query))
    {
        char s[200];
        sprintf(s, "Query to save rake (%9.2f) failed!\n", rake.asDouble());
        Sys_LogError(s);
        Sys_LogError(mysql_error(dbase_->mySQLQuery_));
        return false;
    }

    return true;
}


bool CdbInterface::getShutdown(u_int16_t& shutdown)
{
    SingleLock l(&dbMutex_);
    if (!l.lock())
        return false;

    char query[MAXQUERYSIZE];
    const char* dbShutdown;

    sprintf(query, "SELECT shutdown FROM status");
    
    if (!dbase_->dbQuery(query))
    {
        char s[200];
        sprintf(s, "getShutdown: Query failed!\n");
        Sys_LogError(s);
        Sys_LogError(mysql_error(dbase_->mySQLQuery_));
        return false;
    }

    dbShutdown = dbase_->dbFetchRow();

    if (dbShutdown == NULL)
    {
        // Status record does not exist in database
        return false;
    }

    shutdown = atoi(dbShutdown);

    return true;
}

bool CdbInterface::getShutdownMsg(string& msg)
{
    SingleLock l(&dbMutex_);
    if (!l.lock())
        return false;

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
        // House record does not exist in database
        return false;
    }

    msg = dbShutdownMsg;

    return true;
}

void CdbInterface::dumpHandlog(FILE* fp)
{
    if (logBuf_.size() > 0)
    {
        fwrite(logBuf_.c_str(), 1, logBuf_.size(), fp);
    }
}

bool CdbInterface::dbPing()
{
    SingleLock l(&dbMutex_);
    if (!l.lock())
        return false;

    return dbase_->dbPing();
}


bool CdbInterface::gameHistoryUpdate(CPlayer *player, u_int32_t gamenum)
{
    SingleLock l(&dbMutex_);
    if (!l.lock())
        return false;

    /* Ok.. formulate a query to update player's game history..
       basically, you're shifting a new value into game1.
       game5 <- game4
       game4 <- game3
       game3 <- game2
       game2 <- game1
       game1 <- new value (u_int32_t gamenum)
       store in pokeruser table.   */

    string u = sqlfy(player->getUsername());
    char query[MAXQUERYSIZE];

    memset(query, 0, MAXQUERYSIZE);

    sprintf(query, "UPDATE pokeruser SET game5=game4, game4=game3, game3=game2, game2=game1, game1=%d WHERE username=\"%s\"",
            gamenum, u.c_str());
    
    
    if (!dbase_->dbQuery(query))
    {
        printf("Query to update (shift) %s's gamelog failed on game number [%d]!\n",
            player->getUsername(), gamenum);

        return false;
    }

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


// Log the event, return position of player.
bool CdbInterface::logPlayerLeave(int tournament,
                                  long eventTime,
                                  const char* player,
                                  const char* extra,
                                  int&  position)
{
    SingleLock l(&dbMutex_);
    if (!l.lock())
        return false;

    WriteLockSQLTable sqlLock(dbase_, TOURNAMENTTABLE);
    if (!sqlLock.success_)
        return false;

    bool rc = logTournamentEvent(tournament, TE_PlayerLeave, eventTime, player, extra);

    if (!rc)
        return false;

    /*
        QUERY:

        SELECT COUNT(*) FROM tournamentlog WHERE number=<number> AND
                                                 eventid=<TE_PlayerLeave>;
    */

    ostrstream query;
    query << "SELECT COUNT(*) FROM " << TOURNAMENTTABLE <<
             " WHERE number=" << tournament << " AND event=" <<
             (int)TE_PlayerLeave << ';' << '\0';

    {
        StrmFreezer f(query);
        if (!dbase_->dbQuery(f.str())) 
        {
            Sys_LogError(f.c_str());
            if (player != NULL)
            {
                char s[200];
                sprintf(s, "Player: %s\n", player);
                Sys_LogError(s);
            }
            return false;
        }
    }

    const char* fetched = dbase_->dbFetchRow();

    if (fetched == NULL)
    {
        position = 0;
    }
    else
    {
        position = atoi(fetched);
    }    

    return true;
}

// Return true if player is in the chat ban list,
// false otherwise.
bool CdbInterface::checkChatBan(const char* username)
{
    SingleLock l(&dbMutex_);
    if (!l.lock())
      return false;

    string s = sqlfy(username);

    /*
        QUERY:

        SELECT username FROM chat_ban WHERE username='username';
    */

    ostrstream query;
    query << "SELECT username FROM chat_ban WHERE username='" << s.c_str() << "';" << ends;

    StrmFreezer f(query);
    if (!dbase_->dbQuery(f.str())) 
      return false;

    const char* fetched = dbase_->dbFetchRow();
    if (fetched == NULL)
      return false;

    // username is in the ban list
    return true;
}
