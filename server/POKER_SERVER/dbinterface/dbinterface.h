/**********************************************************************

  NAME:		dbInterface.h
  DATE:		4/9/00
  PURPOSE:	Class CdbInterface implements the functions needed to
			interface with our proprietary database schema.  These
			functions allow us to grab and set information such as
			a player's number of chips, or the game's logs.

**********************************************************************/

#ifndef __table_dbinterface_h__
#define __table_dbinterface_h__

#include "player/player.h"
#include "dbhandler/dbhandler.h"
#include "sync/sync.h"
#include "md5.h"
#include <string>
USING_NS_T(std, string);

#define GAMETABLE       "gamelog"
#define TOURNAMENTTABLE "tournamentlog"

typedef struct gamelog gamelog_t;

struct gamelog
{
	u_int32_t gamenum;
	int numplayers;
};

class CChips;

class CdbInterface
{
public:
	CdbInterface(CdbHandler*);

	bool authenticate(const char*, const char*);
  bool getCity(const char* username, string& city);
	bool getChips(CPlayer*, CChips&);

private:
	bool storeChips(CPlayer*, const CChips&);
public:

  bool openLog();
	bool setLog(u_int32_t, const char*);
  bool flushLog(u_int32_t gameNumber);
	u_int32_t getGameNum(void);
  bool createGameEntry(u_int32_t);

  bool buyinChips(CPlayer* player,
                  CChips chipsRequested,
                  CChips& chipsBoughtIn);
  bool saveChips (CPlayer* player,
                  const CChips& chips);
  bool saveRake  (const CChips& rake);

  bool getShutdown(u_int16_t& shutdown);
  bool getShutdownMsg(string& msg);
  bool dbPing();
  void dumpHandlog(FILE*);
  bool gameHistoryUpdate(CPlayer* player, u_int32_t gamenum);
  bool putHandLogRequest(CPlayer* player, const char* email, int numhands);

  bool logPlayerLeave(int tournament,
                      long eventTime,
                      const char* player,
                      const char* extra,
                      int& position);

  bool checkChatBan(const char* username);

private:
  bool logTournamentEvent(int tournament,
                          int eventId,
                          long eventTime,
                          const char* player,
                          const char* extra);

	CdbHandler *dbase_;
  string logBuf_;

  // Only one thread can access database at a time
  Mutex dbMutex_;
};

#endif 
