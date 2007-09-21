BOOL StateDealPocket2::onCommand(WPARAM wParam, LPARAM lParam)
{
  // It is possible that the server sends the next Action
  // Request PDU before this state has finished its job.
  // We handle that case here by finishing up immediately
  // and setting the state to StateBet
  if (LOWORD(wParam) == ID_PDU_ACTIONREQUEST)
  {
    // Finish up the dealing immediately
    Player* pHouse = pTableView_->getHouse();
    ASSERT(pHouse);
    Player* pPlayer = pTableView_->getPlayer(currentNum_);
    if (!pPlayer)
    {
      pView_->Invalidate(); // left in the middle of deal
    }
    else
    { // Clean up the animation if any
      if (pos_ != CPoint(-1, -1))
      {
        CRect r(pos_, CSize(Base::Cards::Width_,
                            Base::Cards::Height_));
        pView_->InvalidateRect(&r);
      }
    }

    // Finish what we were doing - deal cards.
    while (pPlayer && pPlayer->numCards() != numCards_)
    {
      if (pPlayer->isInGame())
      {
        if (pTableView_->isLocalPlayer(currentNum_))
        { 
          ASSERT(cards_.size() > 0);
          if (cards_.size() > 0)
            MoveCardToPlayer(cards_, pPlayer);
        }
        else
        { 
          Base::Card card;
          pPlayer->addCard(card);
        }
        pTableView_->InvalidateRect(&pPlayer->getCardPaintArea(Player::LastCard));
      }

/*
      Base::Card card;
      if (pTableView_->isLocalPlayer(currentNum_))
      { //
        // Local player's pocket cards are decided by the server
        //
        ASSERT(cards_.size() > 0);
        if (cards_.size() > 0)
        {
          // Pop the first card
          DealCards::iterator it = dealCards_.begin();
          card = (*it);
          dealCards_.erase(it);
        }
      }
      else
      { // 
        // Remote player's cards don't matter since they are
        // not shown until showdown
        //
      }

      if (pPlayer->isInGame())
      {
        // Add the card to player's deck
        pPlayer->addCard(card);
        pTableView_->InvalidateRect(&pPlayer->getCardPaintArea(Player::LastCard));
      }
*/  
      // Prepare to deal to next player
      currentNum_ = pTableView_->getNextPlayerInGame(currentNum_);
      pPlayer = pTableView_->getPlayer(currentNum_);
    }

    // Set the new state immediately;
    // NOTE: GameLogic::setNextState destroys this!
    StateBet* pS = new StateBet();
    GameLogic::Instance().setNextState(pS);
    pS->onCommand(wParam, lParam);
    return TRUE;
  }
  return FALSE;
}
