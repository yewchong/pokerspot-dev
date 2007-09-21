
#include "dbinterface.h"
#include <string>

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



CdbInterface::CdbInterface(CdbHandler *dbase)
{
	dbase_ = dbase;
};


bool CdbInterface::authenticate(char *username, char *password)
{
    SingleLock l(&dbMutex_);
    if (!l.lock())
        return false;

    ReadLockSQLTable sqlLock(dbase_, "pokeruser");
    if (!sqlLock.success_)
        return false;

    char query[MAXQUERYSIZE];
    char *dbPassword;
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
    else if (strcmp(dbPassword, context.hex_digest()))
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


u_int16_t CdbInterface::getChips(CPlayer *player)
{
    SingleLock l(&dbMutex_);
    if (!l.lock())
        return 0;

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

    return (u_int16_t)atoi(dbFetched);
};

bool CdbInterface::storeChips(CPlayer *player, u_int16_t chips)
{
    SingleLock l(&dbMutex_);
    if (!l.lock())
        return false;

    WriteLockSQLTable sqlLock(dbase_, "pokeruser");
    if (!sqlLock.success_)
        return false;

    char query[MAXQUERYSIZE];
    char buffer[20];

    memset(query, 0x0, MAXQUERYSIZE);

    string name = sqlfy(player->getUsername());

    itoa(chips, buffer, 10);
    sprintf(query, "UPDATE pokeruser SET chips='%s' where username='%s'", buffer,
            name.c_str());
    
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

    string uname = sqlfy(player->getUsername());

    char query[MAXQUERYSIZE];
    char fields[MAXROWARRAYSIZE][MAXROWSIZE];
    int i;

    memset(query, 0, MAXQUERYSIZE);
    memset(fields, 0, MAXROWARRAYSIZE * MAXROWSIZE);

    if (numhands == 5)
    {
        sprintf(query, "SELECT game1, game2, game3, game4, game5 FROM pokeruser WHERE username=\"%s\"",
                uname.c_str());
    } else {
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
        if (p == NULL) // XXX: fix crash bug 
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
#ifdef _WIN32
    return true; // XXX: testing tournament!
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

    WriteLockSQLTable sqlLock(dbase_, "gamelog");
    if (!sqlLock.success_)
        return false;

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
bool CdbInterface::setLog(u_int32_t gameNumber, char *charLog)
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

        return (u_int32_t)0;
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
                              u_int16_t chipsRequested,
                              u_int16_t& chipsBoughtIn)
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

    long dbChips = atol(dbFetched);

    if (dbChips < chipsRequested)
    {
        // Player is asking for more chips than he has.
        // This can happen if he's logging into multiple
        // tables at the same time. It is not a problem
        // because we handle it here.
        chipsRequested = dbChips;
    }

    sprintf(query, "UPDATE pokeruser SET chips=%d where username='%s'",
            dbChips - chipsRequested, name.c_str());
    
    bool rc = dbase_->dbQuery(query);
    if (!rc)
    {
        char s[200];
        sprintf(s, "Query to save %s's chips (%d) failed!\n",
                player->getUsername(), dbChips - chipsRequested);
        Sys_LogError(s);
    }
    else
    {   // Success, return number of chips bought in
        chipsBoughtIn = chipsRequested;
    }

    return rc;
}


bool CdbInterface::saveChips(CPlayer* player, u_int16_t chips)
{
    SingleLock l(&dbMutex_);
    l.lock();

// XXX:
// Is the write lock really needed? I read from mysql docs that
// usually you don't need to lock tables explicitely.

    WriteLockSQLTable sqlLock(dbase_, "pokeruser");
    if (!sqlLock.success_)
        return false;

    string name = sqlfy(player->getUsername());

    char query[MAXQUERYSIZE];
    // Atomically increment player's chips in the database by 'chips'.
    sprintf(query, "UPDATE pokeruser SET chips=chips+%d where username='%s'",
            chips, name.c_str());
    
    if (!dbase_->dbQuery(query))
    {
        char s[200];
        sprintf(s, "Query to save %s's chips (%d) failed!\n", player->getUsername(), chips);
        Sys_LogError(s);
        return false;
    }

    return true;
}

bool CdbInterface::saveRake(u_int16_t rake)
{
    SingleLock l(&dbMutex_);
    l.lock();
    
    char query[MAXQUERYSIZE];
    // Atomically increment player's chips in the database by 'chips'.
    sprintf(query, "UPDATE house SET cash=cash+%d", rake);
   
    if (!dbase_->dbQuery(query))
    {
        char s[200];
        sprintf(s, "Query to save rake (%d) failed!\n", rake);
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
