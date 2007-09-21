/**********************************************************************

  NAME:		dbHandler.h
  DATE:		4/7/00
  PURPOSE:	Class CdbHandler implements a wrapper around RDBMS
			(Relational Database Management System) interaction
			and the rest of the server code.

			Currently the class only has support for MySQL systems,
			but it provides the functionality to be extended to
			other RDBMS systems (e.g. Oracle, when PSI Corporation
			is actually churning revenue)

**********************************************************************/

#include "dbhandler/dbhandler.h"

CdbHandler::CdbHandler(DBMS_Type dbType)
{
	dbType_ = dbType;

	memset(query_, 0x00, sizeof(query_));
	memset(dbHost_, 0x00, sizeof(dbHost_));
	memset(username_, 0x0, sizeof(username_));
	memset(password_, 0x0, sizeof(password_));
	memset(database_, 0x0, sizeof(database_));

#ifdef MYSQL_DB
	mySQLQuery_ = NULL;
#endif

#ifdef ORACLE_DB
	oracleQuery_ = NULL;
#endif

};

bool CdbHandler::dbInit(void)
{

#ifdef MYSQL_DB
	return mySQLInit();
#endif

#ifdef ORACLE_DB
	return oracleInit();
#endif
	
	printf("Database Not Supported!\n");

	return false;
};

bool CdbHandler::dbConnect(const char* dbHost, const char* username, const char* password, const char* database)
{
	strncpy(dbHost_, dbHost, MAXHOSTSIZE);
	strncpy(username_, username, MAXNAMESIZE);
	strncpy(password_, password, MAXPASSSIZE);
	strncpy(database_, database, MAXDBSIZE);

	if (!dbInit())
	{
		printf("Couldn't initialize database query structure!\n");
		return false;
	}

#ifdef MYSQL_DB
	return mySQLConnect();
#endif

#ifdef ORACLE_DB
	return oracleConnect();
#endif

	printf("Database Not Supported!\n");

	return false;
};

bool CdbHandler::dbQuery(const char* query)
{
	strncpy(query_, query, MAXQUERYSIZE);
	
	if (!dbPing())
	{
		printf("Unble to ping %s!\n", dbHost_);
		return false;
	}

	if (DEBUG & DEBUG_DATABASE)
	{
		printf("Sending query to %s\n", dbHost_);
	}


#ifdef MYSQL_DB
	return mySQLQuery();
#endif

#ifdef ORACLE_DB
	return oracleSQLQuery();
#endif

	return false;
};

bool CdbHandler::dbPing(void)
{

#ifdef MYSQL_DB
	return mySQLPing();
#endif

#ifdef ORACLE_DB
	return oraclePing();
#endif

	printf("Database Not Supported!\n");

	return false;
};

char *CdbHandler::dbFetchRow(void)
{
#ifdef MYSQL_DB
	return mySQLFetchRow();
#endif

#ifdef ORACLE_DB
	return oracleFetchRow();
#endif

	printf("Database Not Supported!\n");

	return NULL;
};

bool CdbHandler::dbFetchRowArray(char arrayref[MAXROWARRAYSIZE][MAXROWSIZE])
{
#ifdef MYSQL_DB
        return mySQLFetchRowArray(arrayref);
#endif

        printf("Database Not Supported!\n");

        return NULL;
};

void CdbHandler::dbClose(void)
{

#ifdef MYSQL_DB
	mySQLClose();
	return;
#endif

#ifdef ORACLE_DB
	oracleClose();
	return;
#endif

	printf("Database Not Supported!\n");
};


// RDBMS-Specific Functions..  Probably want to put these in seperate files later -RBo

// MySQL RDBMS Specific Functions

#ifdef MYSQL_DB

bool CdbHandler::mySQLInit(void)
{
	mySQLQuery_ = mysql_init(NULL);

	if (mySQLQuery_ == NULL)
		return false;
	else
		return true;
};

bool CdbHandler::mySQLConnect(void)
{
	// Open Connection To MySQL Database

	if (mysql_real_connect(mySQLQuery_, dbHost_, username_, password_,
							database_, 0, NULL, 0) == NULL)
	{
		printf("Unable to connect to MySQL server!\n");
		printf("Host: %s\nUser: %s\n", dbHost_, username_);
		printf("Database: %s\n", database_);
		return false;
	}

	return true;
};

bool CdbHandler::mySQLPing(void)
{
	return (!mysql_ping(mySQLQuery_));
};

bool CdbHandler::mySQLQuery(void)
{
	return(!mysql_query(mySQLQuery_, query_));
};
/* old
char *CdbHandler::mySQLFetchRow(void)
{
	MYSQL_RES *result;
	MYSQL_ROW row;

	result = mysql_store_result(mySQLQuery_);

	if (result == NULL)
	{
		printf("Couldn't store results from previous query!\n");
		return NULL;
	}

	row = mysql_fetch_row(result);
    if (row != NULL)
	    strncpy(fetchedRow_, row[0], MAXROWSIZE);
    else
        return NULL;
	
	mysql_free_result(result);

	return fetchedRow_;
};
*/

char *CdbHandler::mySQLFetchRow(void)
{
        MYSQL_RES *result;
        MYSQL_ROW row;

        result = mysql_store_result(mySQLQuery_);

        if (result == NULL)
        {
                printf("Couldn't store results from previous query!\n");
                return NULL;
        }

        row = mysql_fetch_row(result);
    if (row != NULL)
    {
        if (row[0] != NULL)
                strncpy(fetchedRow_, row[0], MAXROWSIZE);
        else
        {
            mysql_free_result(result);
            return NULL;
        }
    }
    else
    {
        mysql_free_result(result);
        return NULL;
    }

    mysql_free_result(result);

     return fetchedRow_;
};


bool CdbHandler::mySQLFetchRowArray(char arrayref[MAXROWARRAYSIZE][MAXROWSIZE])
{
    MYSQL_RES *result;
    MYSQL_ROW row;
    unsigned int num_fields;
    unsigned int i;

    result = mysql_store_result(mySQLQuery_);

    if (result == NULL)
    {
        printf("Couldn't store results from previous query!\n");
        return false;
    }

    num_fields = mysql_num_fields(result);

    row = mysql_fetch_row(result);

    if (row != NULL)
    {
        for (i = 0; (i < num_fields) && (row[i] != NULL); i++)
        {
            strncpy(arrayref[i], row[i], MAXROWSIZE);
        }
    } else {
        mysql_free_result(result);
        return false;
    }

    mysql_free_result(result);

    return true;
};


void CdbHandler::mySQLClose(void)
{
	mysql_close(mySQLQuery_);
};

#endif	// MYSQL_DB


// Oracle RDBMS Specific Functions

#ifdef ORACLE_DB

bool CdbHandler::oracleInit(void)
{
	printf("Oracle Databases Not Currently Supported!\n");
	return false;
}

bool CdbHandler::oracleConnect(void)
{
	printf("Oracle Databases Not Currently Supported!\n");
	return false;
};

bool CdbHandler::oraclePing(void)
{
	printf("Oracle Databases Not Currently Supported!\n");
	return false;
};

char *CdbHandler::oracleFetchRow(void)
{
	printf("Oracle Databases Not Currently Supported!\n");
	return NULL;
};

void CdbHandler::oracleClose(void)
{
	oracle_close(oracleQuery_);
};

#endif	// ORACLE_DB
