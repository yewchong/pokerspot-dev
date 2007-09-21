/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUTournamentParams
  Created  : 07/11/2000

  OVERVIEW : Tournament parameters PDU. Table server
             sends this PDU to inform about a change
             in the game parameters.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pdutournamentparams.h"
#include "ui/tableview.h"
#include "ui/global.h"
#include "ui/actionbar.h"
#include "ui/cmdid.h"
#include "ui/soundmanager.h"
#include "base/gamelogic.h"
#include "base/statewait.h"
#include "common/stringoutput.h"


BEGIN_NAMESPACE(Network)


void GetBlinds(const CChips& currentBet,
               CChips& smBlind,
               CChips& bigBlind)
{
    switch (currentBet.getDollars())
    {
    case 15:    // rule: 15/30 limits, 10/15 blinds
        smBlind = 10;
        bigBlind = 15;
        break;

    default:
        smBlind = currentBet / 2;
        bigBlind = currentBet;
        break;
    }
}


using Base::GameLogic;
using Base::StateWait;

void PDUTournamentParams::execute(GameState* pS)
{
#ifdef PSPOT_TABLE_MODULE_


    if (!Global::IsTournament())
    {   // Set tournament flag
        Global::SetIsTournament(true);
        if (CActionBar::Instance())
            CActionBar::Instance()->setTournament();
    }

    GameState* state = GameLogic::Instance().currentState();
    if (state)
    {
        // Tell the current state to quit animations if any
        state->onCommand(MAKEWPARAM(ID_FINISH_DEAL, 0), 0);
    }

    if (flag_ == TF_Start)
    {
        GameLogic::Instance().setNextState(new StateWait("The tournament starts!"));
        SndMan::Instance()->playSound(SndMan::SM_Tournament);
    }
    else if (flag_ == TF_GameType)
    {
        // Map server enumerated values to client
        if (p1_ == Game_Type_Holdem)
            p1_ = GT_Holdem;
        else if (p1_ == Game_Type_Omaha)
            p1_ = GT_Omaha;
        else if (p1_ == Game_Type_SevenStud)
            p1_ = GT_SevenStud;
        else
            p1_ = GT_Holdem;

        CTableView* table = CTableView::Instance();

        bool typeChanged = p1_ != Global::GetGameType();
        bool wasHiLo = table ? table->getIsHiLo() : false;
        bool isHiLo = (p2_ != 0) ? true : false;
        bool hiloChanged = (wasHiLo != isHiLo);

        Global::SetGameType((GameType)p1_);

        CString s;
        s.Format("The game is %s", Global::GetGameName());
        if (isHiLo)
          s += " Hi/Lo 8 or better";

        if ((typeChanged || hiloChanged) && table)
        {
            if (typeChanged)
              table->reloadConfiguration();
            table->setIsHiLo(isHiLo);
            table->setCaption();

            GameLogic::Instance().setNextState(new StateWait(s));
        }
    }
    else if (flag_ == TF_Limit)
    {
        bool changed = false;
        CTableView* table = CTableView::Instance();
        if (table)
        {
          CChips lo, hi;
          lo.setRep(p1_);
          hi.setRep(p2_);

            changed = table->lolimit() != lo ||
                      table->hilimit() != hi;
            if (changed)
            {
                table->lolimit(lo);
                table->hilimit(hi);
                table->resetCaption();
            }
        }

        if (changed)
        {
            CStrOut s;
        
            BetStructure bs = Global::GetBetting();
            if (bs == BS_NoLimit)
            {
                CChips lolimit, sb, bb;
                lolimit.setRep(p1_);
                GetBlinds(lolimit, sb, bb);
                s << "The blinds are " << sb << '/' << bb << ". Betting is no-limit.";
            }
            else if (bs == BS_PotLimit)
            {
                CChips lolimit, sb, bb;
                lolimit.setRep(p1_);
                GetBlinds(lolimit, sb, bb);
                s << "The blinds are " << sb << '/' << bb << ". Betting is pot limit.";
            }
            else // (bs == BS_Limit)
            {
                CChips sb, bb;
                sb.setRep(p1_);
                bb.setRep(p2_);
                s << "The limits are " << sb << '/' << bb << '.';
            }
            GameLogic::Instance().setNextState(new StateWait(s.str()));
        }
    }
    else if (flag_ == TF_BetStructure)
    {
        BetStructure bs = BS_Limit;
        if (p1_ == BS_PotLimit)
            bs = BS_PotLimit;
        else if (p1_ == BS_NoLimit)
            bs = BS_NoLimit;

        Global::SetBetting(bs);

        if (bs == BS_Limit)
            GameLogic::Instance().setNextState(new StateWait("Betting is limit based."));
        else if (bs == BS_PotLimit)
            GameLogic::Instance().setNextState(new StateWait("Betting is pot limit."));
        else if (bs == BS_NoLimit)
            GameLogic::Instance().setNextState(new StateWait("Betting is no limit."));
    }
    else if (flag_ == TF_Freeze)
    {
        GameLogic::Instance().setNextState(new StateWait("The tournament is closed from new players."));
    }
    else if (flag_ == TF_Dissolve)
    {
        GameLogic::Instance().setNextState(new StateWait("You will be seated to another table - please wait."));
    }
    else if (flag_ == TF_Pause)
    {
        if (p1_ != 0)
        {
            CString s;
            if (p1_ > 60)
            {
                int mins = p1_ / 60;
                if (mins > 1)
                {
                    s.Format("The tournament is paused %i minutes %i seconds for break.",
                             mins, p1_ - 60 * (p1_ / 60));
                }
                else
                {
                    s.Format("The tournament is paused %i minute %i seconds for break.",
                             mins, p1_ - 60 * (p1_ / 60));
                }
            }
            else
            {
                s.Format("The tournament is paused %i seconds for break.", p1_);
            }
            GameLogic::Instance().setNextState(new StateWait(s));
        }
        else
            GameLogic::Instance().setNextState(new StateWait("The tournament is paused -- please wait"));
    }
#endif
}


END_NAMESPACE(Network)