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

#ifndef __dbHandler_h__
#define __dbHandler_h__

#include "../includes.h"

// RDBMS Type Enabling Defines
#define MYSQL_DB
//#define ORACLE_DB
//#define SYQUEST_DB
//etc..



// Individual RDBMS Includes
#ifdef MYSQL_DB					// MySQL Include File
#include <mysql.h>
#endif

#ifdef ORACLE_DB				// Fictituous Oracle Include File
#include <oracle/oracle.h>
#endif

#define MAXQUERYSIZE	8000
#define MAXHOSTSIZE		128
#define MAXNAMESIZE		40
#define MAXPASSSIZE		40
#define MAXDBSIZE		60
#define MAXROWSIZE		1024
#define MAXROWARRAYSIZE 256

enum DBMS_Type
{
	// Currently only MySQL is supported
	DBMS_TYPE_MYSQL		=	1,
	DBMS_TYPE_ORACLE	=	2
};

class CdbHandler
{
public:
	CdbHandler(DBMS_Type);
	bool dbConnect(char *, char *, char *, char *);
	bool dbInit(void);			// Initialize RDBMS Communications
	bool dbQuery(char *);		// Execute Query to RDBMS System
	char *dbFetchRow(void);		// Fetch Row From Previous Query
    bool dbFetchRowArray(char arrayref[MAXROWARRAYSIZE][MAXROWSIZE]);  // Fetch All Rows Into This Array
	void dbClose(void);			// Close Connection to RDBMS System
	MYSQL *mySQLQuery_;		// MySQL Structure

    // XXX made this public
	bool dbPing(void);			// Tests to see if database server is active

private:
	// Private Methods

	// Individual RDBMS Methods
#ifdef MYSQL_DB			// MySQL-Specific Methods/Variables
	bool mySQLInit(void);			// Initialize MySQL Communications
	bool mySQLConnect(void);		// Connect to MySQL RDBMS
	bool mySQLPing(void);			// Ping MySQL RDBMS
	bool mySQLQuery(void);			// Execute MySQL Query
	char *mySQLFetchRow(void);		// Fetch Row From MySQL Database
    bool mySQLFetchRowArray(char arrayref[MAXROWARRAYSIZE][MAXROWSIZE]);
	void mySQLClose(void);			// Close MySQL Communications


#endif MYSQL_DB

#ifdef ORACLE_DB		// Oracle-Specific Methods/Variables
	bool oracleInit(void);			// Initialize Oracle Communications
	bool oracleConnect(void);		// Connect to Oracle RDBMS
	bool oraclePing(void);			// Ping Oracle RDBMS
	bool mySQLQuery(void);			// Execute Oracle Query
	char *oracleFetchRow(void);		// Fetch Row From Oracle Database
	void oracleClose(void);			// Close Oracle Communications

	ORACLE *oracleQuery_;		// Fictituous Oracle Structure/Object
#endif


	DBMS_Type dbType_;
	char query_[MAXQUERYSIZE];
	char dbHost_[MAXHOSTSIZE];
	char username_[MAXNAMESIZE];
	char password_[MAXPASSSIZE];
	char database_[MAXDBSIZE];
	char fetchedRow_[MAXROWSIZE];

};

#endif