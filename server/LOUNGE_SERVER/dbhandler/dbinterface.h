#ifndef __lounge_dbinterface_h__
#define __lounge_dbinterface_h__

/**********************************************************************

  NAME:		dbInterface.h
  DATE:		4/9/00
  PURPOSE:	Class CdbInterface implements the functions needed to
			interface with our proprietary database schema.  These
			functions allow us to grab and set information such as
			a player's number of chips, or the game's logs.

**********************************************************************/

#include <list>
#include <string>
#include "includes.h"
#include "compat.h"
USING_NS_T(std, string);
USING_NS_T(std, list);


#define TRANS_TYPE_DEBIT    true
#define TRANS_TYPE_CREDIT   false
#define TOURNAMENTTABLE     "tournamentlog"

class CdbHandler;
class CPlayer;
class CChips;
struct pdu_creditcard;
struct CreditCardData;

class CdbInterface
{
public:
  CdbInterface(CdbHandler*);
  ~CdbInterface();
  static CdbInterface* Inst() { return pInst_; }

	bool authenticate(const char*, const char*);
  bool addTransaction(CPlayer*, u_int16_t, u_int16_t, bool);
  bool addDebitTrans(CPlayer*, u_int16_t, u_int16_t);
  bool addCreditTrans(CPlayer*, u_int16_t);
  char *checkTransaction(CPlayer*);

  bool getChips(const CPlayer*, CChips&);
  bool getCreditCards(const CPlayer*, list<pdu_creditcard>&);

  bool getMaxPlayers(long&);
  bool getMotdMsg(string& msg);
  bool getShutdown(u_int16_t& shutdown);
  bool getShutdownMsg(string& msg);

  bool getTournament(int        index,
                     int&       number,
                     CChips&    buyin,
                     CChips&    chips,
                     CChips&    startLo,
                     u_int32_t& startTime,
                     int&       maxPlayers,
                     string&    description,
                     string&    script);
  bool getSatellite (int        index,
                     int&       number,
                     CChips&    buyin,
                     CChips&    chips,
                     CChips&    startLo,
                     int&       numPlayers,
                     string&    description,
                     string&    script);
  bool buyin        (CPlayer*      player,
                     const CChips& chips);
  bool scheduleNewTournament(u_int32_t startTime, int tournamentIndex);
  bool countTournamentPlayers(int tournament,
                              int& joined,
                              int& left);

  bool checkTournament(int tournament);
  bool logTournamentEvent(int tournament,
                          int eventId,
                          long eventTime,
                          const char* player,
                          const char* extra);
  bool setTournamentFormat(const string&);
  bool setTournamentRules(const string&);
  bool checkTournament(const CPlayer&);

  bool createTestPlayers(int numPlayers);
  bool getTestState(int& state);

  bool getCreditCardData(const char* username,
                         list<CreditCardData>& creditCards);
  bool saveCreditCardData(const char* username,
                          const list<CreditCardData>& creditCards);

  bool checkChatBan(const char* username);

private:
	CdbHandler* dbase_;

  char *getCardByIndex(CPlayer *, u_int16_t);

  static CdbInterface* pInst_;
};


inline bool CdbInterface::addCreditTrans(CPlayer* player, u_int16_t amount)
{
  return addTransaction(player, amount, 0, TRANS_TYPE_CREDIT);
}

inline bool CdbInterface::addDebitTrans(CPlayer* player, u_int16_t amount, u_int16_t index)
{
  return addTransaction(player, amount, index, TRANS_TYPE_DEBIT);
}


#endif 
