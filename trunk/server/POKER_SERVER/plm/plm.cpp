/**********************************************************************

  NAME:		plm.cpp
  DATE:		4/8/00
  PURPOSE:	Class Cplm implements the methods needed to actually run a
			poker game.  PLM stands for "Poker Logic Module", which is
			exactly what this class strives to implement: the logic to
			run a poker game.


**********************************************************************/

#include "plm.h"
#include "table/table.h"
#include "pduplay/pdugamenumber.h"
#include "pduplay/pduannounce.h"
#include "tournament/tournament.h"
#include "common/stringoutput.h"
#include "sys.h"

// time allowed for thinking 
#define DEFAULT_THINKING_TIME 20000
// time for timeout 
#define DEFAULT_TIMEOUT_TIME 15000

Cplm::Cplm(CTable* table)
  :
  rake_        (0),
  flopRake_    (false),
  lowRake_     (0),
  hiRake_      (0),
  inGame_      (false),
  table_       (table),
  pgl_         (NULL),
  isFirstRound_(false),
  replyTime_   (DEFAULT_THINKING_TIME),
  timeout_     (DEFAULT_THINKING_TIME + DEFAULT_TIMEOUT_TIME),
  upCards_     (0),
  sequenceNumber_(0),
  highBetter_  (NULL),
  lastToAct_   (NULL),  
  hiLo_        (false),   
  betStructure_(BS_Limit) 
{
  memset((void *)&currentInstr_, 0x0, sizeof(pgl_instr_t));
  FILE* inFile = NULL;
    
  switch (table_->getGameType())
  {
    case (Game_Type_Holdem) : inFile = fopen(HOLDEM_FILE, "r"); break;
    case (Game_Type_SevenStud) : inFile = fopen(SEVENSTUD_FILE, "r"); break;
    case (Game_Type_Omaha) : inFile = fopen(OMAHA_FILE, "r"); break;
    default: printf("GameType %d not supported!\n", table_->getGameType()); break;
  }
    
  ASSERT(inFile != NULL);

  pgl_ = new Cpgl(inFile);
    
  if (inFile != NULL)
    fclose(inFile);    
}

Cplm::~Cplm()
{
  delete pgl_;
  pgl_ = NULL;
}

void Cplm::ante()
{
    if (0)//DEBUG & DEBUG_PLM_ANTE)
    {
        printf("Doing ANTE Function: ");

        if (currentInstr_.operand & iANTE_BLINDS)
        {
            printf("Getting Ante From Blinds Posters.\n");
        } else if (currentInstr_.operand & iANTE_HI_LIMIT)
        {
            printf("Getting Hi-Limit From All Players For Ante.\n");
        } else if (currentInstr_.operand & iANTE_LO_LIMIT)
        {
            printf("Getting Lo-Limit From All Players For Ante.\n");
        } else {
            printf("Getting %d Chips From All Players For Ante.\n",
                currentInstr_.operand & iANTE_MASK);
        }       
    }
    doAnte();
}

void Cplm::deal()
{
    if (table_->countPlayers(PLAYER_STATE_PLAYING|
                             PLAYER_STATE_ALLIN|
                             PLAYER_STATE_ALLIN_RAISE) < 2)
        return; 

    int numCards = (currentInstr_.operand & iDEAL_MASK);

    if (DEBUG & DEBUG_PLM_DEAL)
    {
        //printf("Doing DEAL Function: ");

        if (currentInstr_.operand & iDEAL_UP)
        {
            //printf("Dealing %d Cards Face Up\n", numCards);
            doDealUpCards(numCards); // XXX 7-stud
        }
        else if (currentInstr_.operand & iDEAL_DOWN)
        {
            //printf("Dealing %d Cards Face Down\n", numCards);
            doDealPocketCards(numCards);
        }
        else if (currentInstr_.operand & iDEAL_INDIAN)
        {
            //printf("Dealing %d Cards Indian-Style\n", numCards);
        }
        else if (currentInstr_.operand & iDEAL_COMMUNITY)
        {
            //printf("Dealing %d Community Cards\n", numCards);
            doDealCommunityCards(numCards);

            if (table_->countPlayers(PLAYER_STATE_PLAYING) < 2 &&
                table_->getBoard() && table_->getBoard()->getNumCards() < 5)
            {
              // We don't have active players for betting round any more -
              // sleep a bit so the community cards will appear a bit slower
              // and not all at once
              Sys_Sleep(3000);
            }
        }
        else
        {
            //printf("Malformed Operand!\n");
        }       
    }
}

void Cplm::draw()
{
    if (DEBUG & DEBUG_PLM_DRAW)
    {
        printf("Doing DRAW Function\n");
    }
}

void Cplm::bet()
{
  int state = PLAYER_STATE_PLAYING;
  if (CTournament::Inst()->isTournament() &&
      isFirstRound_)
  {
    // In tournaments blinds can go all-in
    state |= (PLAYER_STATE_ALLIN|PLAYER_STATE_ALLIN_RAISE);
  }
  
  if (table_->countPlayers(state) < 2)
    return; 
  
  int numBets = (currentInstr_.operand & iBET_CAP_MASK);
  CChips raise = table_->getLo();
  
  if (currentInstr_.operand & iBET_NO_LIMIT)
    raise = 0;
  else if (currentInstr_.operand & iBET_HI_LIMIT)
    raise = table_->getHi();
  else if (currentInstr_.operand & iBET_LO_LIMIT)
    raise = table_->getLo();
  else if (currentInstr_.operand & iBET_POT_LIMIT)
    raise = -1;
  
  if (0)//DEBUG & DEBUG_PLM_BET)
  {
    printf("Doing BET Function: ");
    if (currentInstr_.operand & iBET_NO_LIMIT)
    {
      printf("No Limit");
    }
    else if (currentInstr_.operand & iBET_HI_LIMIT)
    {
      printf("Hi Limit (%d)", table_->getHi());
    }
    else if (currentInstr_.operand & iBET_LO_LIMIT)
    {
      printf("Lo Limit (%d)", table_->getLo());
    }
    else if (currentInstr_.operand & iBET_POT_LIMIT)
    {
      printf("Pot Limit");
    }
    else
    {
      printf("Malformed Operand!\n");
    }
    
    if (currentInstr_.operand & iBET_CAP)
    {
      printf(", Bet is Capped at %d Bets\n", numBets);
    }
    else
    {
      printf("\n");
    }
  }
  doBettingRound(numBets, raise);
}

void Cplm::showdown()
{
  if (0)//DEBUG & DEBUG_PLM_SHOWDOWN)
  {
    printf("Doing SHOWDOWN Function: ");
    
    if ((currentInstr_.operand & iSHOWDOWN_HI) &&
      (currentInstr_.operand & iSHOWDOWN_LO))
    {
      printf("Pot Will Be Split Between Hi and Lo Hands");
    } else if (currentInstr_.operand & iSHOWDOWN_HI) {
      printf("Pot Will Be Awarded To Hi Hand");
    } else if (currentInstr_.operand & iSHOWDOWN_LO) {
      printf("Pot Will Be Awarded To Lo Hand");
    }
    
    if (currentInstr_.operand & iSHOWDOWN_GUTS)
    {
      printf(", This is a Guts Game\n");
    }
    else
    {
      printf("\n");
    }
  }
  
  doShowdown();
  table_->advanceButton();
  doCleanup();
}

void Cplm::mainLoop()
{
    CStrOut message;
    int i = 0;

    // If a player left in the middle of hand, it might
    // happen that the pot still has money - if so, give
    // the chips here to the player

    if (!inGame_ &&
         (table_->countPlayers(PLAYER_STATE_ACTIVE) < 2 ||
          table_->numPlayersSittingIn() < 2 ||
          CTournament::Inst()->pause()))
    {
        doCleanup();
        Sys_Sleep(2000);

        if (!CTournament::Inst()->isTournament())
        {
          // Players are sitting out
          CpduAnnounce pdu(table_);
          pdu.sendAnnounce("Waiting for players to sit in");
          Sys_Sleep(2000);
        }

        return;
    }
    else
    {
        // The game thread function calls this repeatedly.
        // It only gets this far when there's enough players for a game.
        // First off, get the game number, and setLog all of the current players.

        memcpy((void*)&currentInstr_, pgl_->fetchInstr(), sizeof(pgl_instr_t));

        switch (currentInstr_.opcode)
        {
        case (Instr_NOP):
          break;

        case (Instr_Deal):
          deal();
          break;  // Deal Cards To All Active Players

        case (Instr_Ante):                      // Do ANTE
        {
            table_->setGameNumber();
            CpduGameNumber::SendGameNumber(table_);
            for (i = 0; i < 10; i++)
            {
                CPlayer* player = table_->getPlayerFromSlot(i);
                if (player)
                {
                  message << CStrOut::Clear
                          << "Seat " << i << ": "
                          << player->getUsername() 
                          << " (" << player->getChips() 
                          << " in chips)";
                  table_->setLog(message.str());
                }
            }

            ante();
        }
        break;

        case (Instr_Draw):
          draw();
          break;       // Allow Players to Draw Cards

        case (Instr_Bet):
          bet();
          break;       // Do a betting Round

        case (Instr_Showdown):
          showdown();
          break;       // Do Showdown/Announce

        default:
          printf("Unknown operation %d!\n", (int)currentInstr_.opcode);
          break;
        }
    }
}


//
// Pre-increment current sequence number.
//
u_int16_t Cplm::incSequenceNumber()
{
    ++sequenceNumber_;

    if (sequenceNumber_ == PDU_NOT_SEQUENCED)
    {   
        // Restart sequencing from 0.
        //
        // NOTE that in theory if a pdu travels for so long
        // that the sequence number wraps around then it is
        // possible that a wrong pdu is interpreted as a reply
        // to an action. In practice, that would require the
        // pdu to travel for a very long time (> 100 hands)
        // during which time the player would be thrown out
        // anyway.

        sequenceNumber_ = 0;
    }

    return sequenceNumber_;
}

