#ifndef __table_plm_h__
#define __table_plm_h__

/**********************************************************************

  NAME:		plm.h
  DATE:		4/8/00
  PURPOSE:	Class Cplm implements the methods needed to actually run a
			poker game.  PLM stands for "Poker Logic Module", which is
			exactly what this class strives to implement: the logic to
			run a poker game.


**********************************************************************/

#include "includes.h"
#include "pgl/pgl.h"
#include "dbinterface/dbinterface.h"
#include "winner.h"

class CTable;
class CPlayer;
class Cpot;
class CChips;

#define HOLDEM_FILE			    "HOLDEM.PGL"
#define SEVENSTUD_FILE		  "7STUD.PGL"
#define SEVENSTUDHILO_FILE	"7STUDHILO.PGL"
#define OMAHA_FILE			    "OMAHA.PGL"

enum Plm_Action
{
  Action_Timeout  = 0,
  Action_Fold     = 1,
  Action_Call     = 2, // no need for separate check action
  Action_Raise    = 3,
  Action_Ante     = 4,
  Action_SitOut   = 5,
  Action_SitIn    = 6,
  Action_AnteAllIn = 7
};


class Cplm
{
public:
  Cplm(CTable*);
  virtual ~Cplm();

  virtual void mainLoop();
  virtual void ante();
  virtual void deal();
  virtual void draw();
  virtual void bet();
  virtual void showdown();

  bool isHiLoSplit() const; 
  void setHiLoSplit(bool);

protected:
  //
  // Ante mfunctions
  //
  virtual void doAnte();
  CPlayer* decideButtonPlayer(int* asked);
  Plm_Action promptBlindPost(CPlayer*, const CChips& pay, bool isSmallBlind);
  Plm_Action promptSitIn(CPlayer*);
  void actionAnte(CPlayer*, const CChips& pay, bool isMissed = false);
  void actionSitOut(CPlayer*);
  void actionSitIn(CPlayer*);
  void potAnte(CPlayer *, const CChips&);
  
  // 
  // Betting mfunctions
  //
  virtual void doBettingRound(int cap, const CChips& limit);
  Plm_Action promptAction(CPlayer*, const CChips& toPay, const CChips& raise);
  Plm_Action promptActionSpread(CPlayer*, const CChips& toPay, CChips& raise);
  void actionTimeout(CPlayer*, const CChips& pay);
  void actionFold(CPlayer*);
  void actionCall(CPlayer*, const CChips& pay, bool isAnte = false);
  void actionRaise(CPlayer*, const CChips& raise, const CChips& realRaise, bool isBringIn = false);
  void echoAction(CPlayer*, Plm_Action, const CChips& value);
  void potCall(CPlayer*);
  void potRaise(CPlayer*, const CChips&);
  void potFold(CPlayer*);
  void potEndRound();
  void potEndGame();
  virtual bool isLastBettingRound();
  
  //
  // All-In mfunctions
  //
  void insertPot();
  void splitPot(CPlayer*, const CChips& pay);
  void allIn(CPlayer *, const CChips& pay);
  Cpot* splitPot(Cpot*, CPlayer*, const CChips&, const CChips&);
  
  //
  // Deal mfunctions
  //
  virtual void doDealPocketCards(int);
  virtual void doDealUpCards(int); 
  virtual void doDealCommunityCards(int);
  
  //
  // Showdown mfunctions
  //
  virtual void doShowdown();
  int announcePot(Cpot*, int potNum, Winners& hi, Winners& lo);
  CPlayer* getPlayerClosestToLeftOfDealer(Winners&);
  virtual CPlayer* getOddChipWinner(Winners&);
  void distributeChips(const CChips& potChips, Winners& winners);
  void foldLoggedOut();
  
  //
  // Rake mfunctions
  virtual void doRake();
  void saveRake();
  void resetRake();
  
  //
  // Cleanup mfunctions
  //
  virtual void doCleanup();
  void rebuildRing();
  void handleNetworkErrors(CPlayer*);
  void doShutdown(); 
  bool checkShutdown();
  void doTournament();
  
  u_int16_t getSequenceNumber() const;
  u_int16_t incSequenceNumber();
  
  void sendChipUpdates();
  void sendChipUpdates(CPlayer*);

protected:
  bool inGame_;
	CTable* table_;
  bool isFirstRound_;
  CPlayer* highBetter_;

  DWORD replyTime_;
  DWORD timeout_;
  
  int upCards_;
  
  CChips    rake_;    // chips in rake
  bool      flopRake_;// flop rake taken?
  CChips    lowRake_; // rake when pot reaches this 
  CChips    hiRake_;  // rake when pot reaches this 
  
  Cpgl* pgl_;
  pgl_instr_t currentInstr_;
  
  u_int16_t sequenceNumber_;
  
  CPlayer* lastToAct_;
  
  bool hiLo_;         
  
public:
  enum BetStructure
  {
    BS_Limit    = 1,
      BS_PotLimit = 2,
      BS_NoLimit  = 3
  };
  
  BetStructure getBetStructure() const;
  bool isSpreadLimit() const;
  void setBetStructure(BetStructure);
  
private:
  BetStructure betStructure_;    
};

inline u_int16_t Cplm::getSequenceNumber() const
{
  return sequenceNumber_;
}

inline bool Cplm::isHiLoSplit() const
{
  return hiLo_;
}

inline void Cplm::setHiLoSplit(bool b)
{
  hiLo_ = b;
}


#endif 
