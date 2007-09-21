//
// STATE: Wait a hand to start.
// 
// Entry: Joined a table or showdown of previous hand
//        has ended
//
// PDUs:  first PDU_BlindQuery or PDU_SitInQuery,
//        then PDU_DealCards
//
// Exit:  StateDeal on PDU_DealCards
//

BOOL StateHandStart::onPDU(PDUHeader* pPDU)
{
  BOOL rc = FALSE;

  BOOL isMe = pPDU->username_ == g_UserName;

  if (isMe)
  {
    if (IsPDU(pPDU, PDU_BlindQuery))
    {
      rc = TRUE;
      onBlindQuery(pPDU);
      delete pPDU;
    }
    else if (IsPDU(pPDU, PDU_SitInQuery))
    {
      rc = TRUE;
      onSitInQuery(pPDU);
      delete pPDU;
    }
    else if (IsPDU(pPDU, PDU_DealCards))
    {
      PDUDealCards* pDC = static_cast<PDUDealCards*>(pPDU);
      GameState* pNewState = new StateDealPocketCards(
                                   pDC->card1,
                                   pDC->suit1,
                                   pDC->card2,
                                   pDC->suit2,
                                   pDC->time);
      delete pDC;
      rc = TRUE;
      GameLogic::setNextState(pNewState);
    }
  }

  if (!rc)
    rc = GameState::onPDU(pPDU);

  return rc;
}


BOOL StateHandStart::onBlindQuery(PDUBlindQuery* pBQ)
{
  if (GameLogic::SitOut())
  { // User wants to sit out - send Sit-Out PDU
    Network::SendPDUSitOut();
  }
  else if (GameLogic::AutoPostBlind())
  { // User has auto post of blind on - send Post Blind PDU
    Network::SendPDUPostBlind();
  }
  else
  {
    // Prompt user whether to post the blind!
    startPromptUser(pBQ->time);
  }

  return TRUE;
}


BOOL StateHandStart::onSitInQuery(PDUSitInQuery* pSIQ)
{
  if (GameLogic::SitBack())
  {
    Network::SendPDUSitBack();
  }
  else 
  {
    Network::SendPDUSitIn();
  }
  return TRUE;
}



BOOL StateHandStart::tick(DWORD dt)
{
  if (promptUser_)
  {
    ticks_ += dt;
    if (ticks_ < timer_)
    {
      CActionBar::animate(dt);
    }
    else
    {
      endPromptUser();
    }
  }
}


BOOL StateHandStart::onCommand(WPARAM wParam, LPARAM lParam)
{
  BOOL rc = FALSE;
  
  if (LOWORD(wParam) == ID_BTN_1)
  {
    Network::SendPDUPostBlind();
    endPromptUser();
    rc = TRUE;
  }

  if (!rc)
    rc = GameState::onCommand(wParam, lParam);

  return rc;


}


void StateHandStart::startPromptUser(DWORD time)
{
  timer_ = time;
  promptUser_ = TRUE;
  CActionBar::Instance()->setButtonText(0, "Post Blind");
  CActionBar::Instance()->enableButtons(1);
}


void StateHandStart::endPromptUser()
{
  timer_ = 0;
  promptUser_ = FALSE;
}

//
// STATE: Base game state
// 

//
// Base game state does not recognize any PDUs
// but passes the message on to the active view.
//
BOOL GameState::onPDU(PDUHeader* pPDU)
{
  return FALSE;
}


