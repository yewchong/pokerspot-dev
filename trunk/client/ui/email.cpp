	ON_COMMAND(ID_SUPPORTEMAIL, OnSupportemail)
	ON_UPDATE_COMMAND_UI(ID_SUPPORTEMAIL, OnUpdateSupportemail)
	ON_COMMAND(ID_PAGEFLOORMAN, OnPagefloorman)
	ON_UPDATE_COMMAND_UI(ID_PAGEFLOORMAN, OnUpdatePagefloorman)

const char* g_szMapiMessage =
"You need to configure your email client so it can be used by MAPI-based applications.\n"
"Please consult your email software manual on how to do this.";


void CMainWnd::OnSupportemail() 
{
  if (!CMailDocument::IsMAPIAvailable())
  {
    AfxMessageBox(g_szMapiMessage);
    return;
  }


    CMailDocument doc("SMTP:support@pokerspot.com");
    if (CTableView* table = CTableView::Instance())
    {
      int tableNumber = table->getTableNumber(),
          gameNumber = table->getGameNumber(),
          previousGame = table->getPrevGameNumber();

      CString s;
      s.Format("Username: %s\r\nTable: %i %s\r\nGame #%i, Previous #%i",
               Network::Server::Username_,
               tableNumber, table->getGameTitle(),
               gameNumber, previousGame);
      doc.setBody(s);
      doc.setSubject("Support Request");
    }
    doc.sendMail();
}

void CMainWnd::OnUpdateSupportemail(CCmdUI* pCmdUI) 
{
  //pCmdUI->Enable(CMailDocument::IsMAPIAvailable());
  pCmdUI->Enable(TRUE);
}

void CMainWnd::OnPagefloorman() 
{
  if (!CMailDocument::IsMAPIAvailable())
  {
    AfxMessageBox(g_szMapiMessage);
    return;
  }

    CMailDocument doc("SMTP:rboyd@pokerspot.com");

    CString body;

    if (CTableView* table = CTableView::Instance())
    {
      int tableNumber = table->getTableNumber(),
          gameNumber = table->getGameNumber(),
          previousGame = table->getPrevGameNumber();

      body.Format("Requesting floorman: %s\r\nTable: %i %s\r\nGame #%i, Previous #%i",
                  Network::Server::Username_,
                  tableNumber, table->getGameTitle(),
                  gameNumber, previousGame);
    }
    
    doc.setSubject("Requesting floorman");
    doc.setBody(body);
    doc.sendMail(); 
}

void CMainWnd::OnUpdatePagefloorman(CCmdUI* pCmdUI) 
{
  //pCmdUI->Enable(CMailDocument::IsMAPIAvailable());
  pCmdUI->Enable(TRUE);
}
