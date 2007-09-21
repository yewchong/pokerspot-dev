/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Class    : Cplm_7Stud
  Created  : 06/21/2000

  OVERVIEW : 7 Card Stud logic module. This is a subclass
             of the basic Cplm that implements 7 Card Stud
             specific ante and betting structure.

 ****************************************************************************/

#include "plm_7stud.h"
#include "table/table.h"
#include "ring/ring.h"
#include "pdunotify/pdunotify.h"
#include "pduplayer/pduplayer.h"
#include "pduplay/pduannounce.h"
#include "pduplay/pdubutton.h"
#include "pduplay/pdublindrequest.h"

//
// Ante mfunctions
//
void Cplm_7Stud::doAnte()
{
    CChips ante = 1;
    CChips bringin = 2;
    int numAntes = 0;

    if (table_->getLo() < 10)
    {
        ante = 1;
        bringin = 2;
    }
    else
    {
        ante = 2;
        bringin = 5;
    }

    // Remember the players who refused button so
    // they won't be asked twice for sit-in
    // NOTE: This would be a whole lot simpler if the
    // button player didn't have to sit-in
    int buttonRefused[10];
    memset(buttonRefused, 0, sizeof(buttonRefused));

    CPlayer* player = getButtonPlayer(buttonRefused);
    if (!player)
    {
        printf("doAnte(): no button player\n");
        doCleanup();
        return;
    }

    // We need at least two blinds to start a game -
    // remember the first blind in case game won't start
    // and the blind has be refunded.
    CPlayer* firstBlindPlayer = NULL;

    char message[100];

    CRingIterator it(table_->getRingNode(player));

    // Players in these states can join next hand
    int state = PLAYER_STATE_WAITING|PLAYER_STATE_READY; 

    // start queries from player next to "button"
    ++it;
    if (!it.restart(state))
    {
        printf("doAnte: not enough players\n");
        doCleanup();
        return;
    }

    //
    // Get blinds from all players
    //
    sendBlindRequests(it.current()->getPlayer(), ante);

    do {

        if (table_->countPlayers(state|PLAYER_STATE_PLAYING) <= 1)
            break; // not enough players to start game

        player = it.current()->getPlayer();

        if (buttonRefused[player->getSlot()])
        {
            // This player already refused the button -
            // skip him
            continue;
        }


        Plm_Action action = Action_Timeout;

        // Player must have at least ante + bringin chips to play
        if (player->getChips() == 0 || player->getChips() < ante + bringin)
        {
            CpduNotify pdu;
            pdu.sendNotifyMessage(player->getConnfd(),
                                  "You do not have enough chips to join the game.",
                                  NF_NoCloseConnection|NF_StatusMessage);

            action = Action_SitOut;
        }
        else 
        {
            // Ask the player for the blind
            //action = promptBlindPost(player, ante, FALSE);
            action = getBlindPost(player, ante);
        }

        switch (action)
        {
            case Action_Timeout:
            {   // Timeout on blind request => sitout
                actionSitOut(player);
                sprintf(message, "%s : Sit Out (Timed Out).",
                        player->getUsername());
                table_->setLog(message);
                memset(message, 0x0, sizeof(message));

		        // Change player's state to NOTRESPONDING -
		        // ring rebuild will take care of player
                player->stateChange(PLAYER_STATE_NOTRESPONDING);               
            }
            break;

            case Action_SitOut:
            {
                // Increment sitout counter. Eventually he will be thrown away
                // unless he pays and plays
                player->setSitouts(player->getSitouts() + 1);

                actionSitOut(player);
                sprintf(message, "%s : Sit Out", player->getUsername()); 
                table_->setLog(message);
                memset(message, 0x0, sizeof(message));
            }
            break;

            case Action_Ante:
            {
                numAntes++;
                if (numAntes == 1)
                    firstBlindPlayer = player;

                player->setSitouts(0);

                actionAnte(player, ante);

                // If player antes.. update his game history
                table_->gameHistoryUpdate(player);
            }
            break;

            default:
            {
                char buf[128];
                sprintf(buf, "Cplm::doAnte: unknown action %d player %s\n",
                        action, player->getUsername());
                Sys_LogError(buf);

                // SIT OUT
                player->setSitouts(player->getSitouts() + 1);
                actionSitOut(player);
                sprintf(message, "%s : Sit Out (Unknown action)", player->getUsername()); 
                table_->setLog(message);
                memset(message, 0x0, sizeof(message));
            }
            break;
        }
    }
    while (it.next(state)); 

    if (numAntes < 2)
    {
        // Cannot start hand because big blind was not paid.
        // If small blind did pay, refund the money and wrap it up

        if (firstBlindPlayer != NULL)
        {
            firstBlindPlayer->setChips(firstBlindPlayer->getChips() + ante);
            CpduPlayer pdu(table_);
            pdu.sendPlayerPdu(firstBlindPlayer, PLAYER_CHIPUPDATE);
        }

        CpduAnnounce pduAnnounce(table_);   
        pduAnnounce.sendAnnounce("Insufficient ante to start game!");

        // To prevent infinite loop in degenerate cases when a hand
        // never starts because no-one posts blinds, clear missed blind
        // flags for sitin players!
        CRingIterator it(table_->getRing());
        if (it.start(state|PLAYER_STATE_PLAYING))
        {
            do
            {
                CPlayer* p = it.current()->getPlayer();
                if (p->getSitIn())
                    p->setMissedBlind(0);
            }
            while (it.next(state|PLAYER_STATE_PLAYING));
        }

        doCleanup();
    }
    else
    {
        // hand starts
        inGame_ = true;
    }

    // The first betting round is different from other rounds
    isFirstRound_ = true;
}


// Button player decision. In 7-stud, the button is always
// the first player who sits in.
CPlayer* Cplm_7Stud::getButtonPlayer(int* refused)
{
    CRingIterator it(table_->getDealer());

    // Players in these states can join next hand
    int state = PLAYER_STATE_WAITING|PLAYER_STATE_READY; 

    // start queries from the "button" player 
    if (!it.restart(state))
    {
        return NULL;
    }

    CpduButton buttonpdu(table_);
    buttonpdu.sendButton();

    // Return the first player who is ready to play
    return it.current()->getPlayer();
}


void Cplm_7Stud::sendBlindRequests(CPlayer* player, const CChips& toAnte)
{
    // In 7-Stud, all players reply to the same request
    CpduBlindRequest blindRequest(table_);
    blindRequest.sendBlindRequest(player, 
                                  toAnte,
                                  replyTime_,
                                  incSequenceNumber());
}


Plm_Action Cplm_7Stud::getBlindPost(CPlayer* player, const CChips& toAnte)
{
    if (player->getChips() < toAnte)
        return Action_SitOut;

    Plm_Action rc = Action_Timeout;

    Cpdu* pdu = NULL;
    if (player->waitPdu(&pdu, timeout_, getSequenceNumber()) == CPlayer::Pdu_Wait_Timeout)
    {
        player->setMissedBlind(toAnte);
        return Action_Timeout;
    }
    else
    {
        // The pdu must be either blind post or sit out pdu. If not,
        // the client is not obeying the protocol.
        if (!pdu)
        {
            Sys_LogError("Cplm_7Stud::getBlind: unexpected NULL pdu.");
        }
        else if (pdu->isPdu(PDU_Play_PostBlind))
        {
            player->setMissedBlind(0);
            rc = Action_Ante;
        }
        else if (pdu->isPdu(PDU_Play_RefuseBlind))
        {
            player->setMissedBlind(toAnte);
            rc = Action_SitOut;
        }
        else if (pdu->isPdu(PDU_Play_SitOut))
        {   // deprecated!
            player->setMissedBlind(toAnte);
            rc = Action_SitOut;
        }
        else
        {
            char buf[128];
            sprintf(buf, "Cplm_7Stud::getBlindPost: unexpected pdu type: %d from player %s.\n",
                    pdu->getType(), player->getUsername());
            Sys_LogError(buf);
        }

        // Pdu has been handled, deallocate
        delete pdu;
        pdu = NULL;
    }

    return rc;
}
