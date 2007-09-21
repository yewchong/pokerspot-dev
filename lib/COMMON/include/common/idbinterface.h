#ifndef __cwg_dbinterface_h__
#define __cwg_dbinterface_h__

/****************************************************************************

  Copyright (c) PSI Corp, 2001

  Title    : interface IDBInterface
  Created  : 09/09/2001

  OVERVIEW : This abstract class defines the application's interface
             to the database. To implement this interface for a particular
             database, create a subclass of IDBInterface and implement the
             member functions.

 ****************************************************************************/


// XXX NOTE: should create separate interfaces for lounge and table servers???

#include <wintypes.h>
#include <list>
#include <string>


// The mfunctions result is of type DBRESULT
// Value S_OK denotes success.
typedef LONG DBRESULT;

class CChips;
class CPlayer;
class CCreditCard;


class IDBInterface
{
public:
  static IDBInterface* Inst() { return pInst_; }
  static void SetInstance(IDBInterface* pi) { pInst_ = pi; }

public:

  //
  // Player data
  //
  virtual DBRESULT authenticate     (const char* username,
                                     const char* password) = 0;
  virtual DBRESULT getCity          (const char* username,
                                     string& city) = 0;
  virtual DBRESULT getChips         (const char* username,
                                     CChips&) = 0;
  virtual DBRESULT buyinChips       (const char* username,
                                     const CChips& requested,
                                     CChips& boughtIn) = 0;
  virtual DBRESULT saveChips        (const char* username,
                                     const CChips& chips) = 0;
  virtual DBRESULT checkChatBan     (const char* username,
                                     bool& banned) = 0;
  virtual DBRESULT gameHistoryUpdate(const char* username,
                                     ULONG gamenum)  = 0;
  virtual DBRESULT putHandLogRequest(const char* username,
                                     const char* email,
                                     int numHands) = 0;
  virtual DBRESULT getCreditCards   (const char* username,
                                     list<CCreditCard>&) = 0;
  virtual DBRESULT saveCreditCards  (const char* username,
                                     const list<CCreditCard>& creditCards) = 0;


  //
  // Game data
  //
  virtual DBRESULT openLog        (ULONG gameNumber) = 0;
  virtual DBRESULT logGameAction  (ULONG gameNumber /* parameters to be defined */) = 0;
  virtual DBRESULT flushLog       (ULONG gameNumber) = 0;
  virtual DBRESULT createGameEntry(ULONG&) = 0;
  virtual DBRESULT saveRake       (const CChips&) = 0;


  //
  // Tournament data
  //
  virtual DBRESULT getTournament       (int&          number,
                                        u_int16_t&    buyin,
                                        u_int16_t&    startChips,
                                        u_int16_t&    startLo,
                                        u_int32_t&    startTime,
                                        string&       description,
                                        string&       scriptName) = 0;
  virtual DBRESULT logPlayerElimination(int           tournament,
                                        long          eventTime,
                                        const char*   player,
                                        const char*   extra,
                                        int&          position) = 0;
  virtual DBRESULT logTournamentEvent  (int          tournament,
                                        int          eventId,
                                        long         eventTime,
                                        const char* player,
                                        const char* extra);
  virtual DBRESULT tournamentBuyin     (const char*   player,
                                        const CChips& chips) = 0;

  // Misc
  virtual DBRESULT getMotd       (string& msg) = 0;
  virtual DBRESULT getShutdown   (UINT& shutdown) = 0;
  virtual DBRESULT getShutdownMsg(string& msg) = 0;
  virtual DBRESULT dbPing        (bool& pingOk) = 0;
};

#endif 
