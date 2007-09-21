/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : layout.cpp
  Created  : 25.11.1999

  OVERVIEW : Implements loading of table view configuration from a text file.

 ****************************************************************************/

#include "stdafx.h"
#include "base/layout.h"
#include "base/player.h"
#include "base/cards.h"
#include "tem/playerindex.h"
#include "ui/tableview.h"
#include "ui/chatview.h"
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



namespace Base
{

using namespace std;
typedef istream_iterator<string, char, char_traits<char> > string_input;
typedef vector<string> strvector;

void lowercase(string& s)
{
  for (int i = 0; i < s.size(); i++)
    s[i] = tolower(s[i]);
}


CPoint MakePoint(const string& s)
{
  CPoint pos(0, 0);

  int i = s.find_first_of(',');

  if (i != string::npos && s.size() > i)
  {
    pos = CPoint(atoi(s.c_str()), atoi(&s[i+1]));
  }

  return pos;
}

//
// If there are spaces in a coordinate entry in the
// layout file, then those coordinates will be two
// entries in the 'data' vector because a space is
// a string separator.
//
// This function collapses such entries into one to
// allow entries like: '13, 5' in the layout file.
//
void collapse(strvector& data)
{
  strvector tmp;
  for (strvector::const_iterator i = data.begin(),
                                 e = data.end();
       i != e;
       ++i)
  {
    string s = *i;
    int pos = s.find_first_of(',');

    if (pos == string::npos || pos == (s.size() - 1) &&
        i != e)
    {
      ++i;
      string s2 = *i;
      s += s2;
    }

    tmp.push_back(s);
  }

  data = tmp;
}


bool error(CTableView* pView, const CString& s)
{
  pView->MessageBox(s);
  return false;
}


typedef bool (*token_handler)(strvector::const_iterator&,
                              strvector::const_iterator&,
                              CTableView*);

// token: '/*'
// skip tokens until '*/'
bool token_comment(strvector::const_iterator& i,
                   strvector::const_iterator& end,
                   CTableView* pView)
{
  bool endComment = false;

  for (++i;
       i != end && !endComment;
       ++i)
  {
    string s = *i;
    endComment = (s == "*/");
  }

  if (!endComment)
    return error(pView, "End of file without ending */");

  // Point to end of comment
  --i;

  return endComment;
}

bool ReadPos(strvector::const_iterator& i,
             const strvector::const_iterator& end,
             string& s)
{
  if (i == end) return false;

  s = *i++;
  int pos = s.find_first_of(',');

  if (pos == string::npos || pos == (s.size() - 1) &&
      i != end)
  {
    if (i == end) return false;
    string s2 = *i++;
    s += s2;
  }

  return s.size() > 0;
}


// token:     'playerpos'
// slot:      int 
// labelpos:  int, int
// opencard:  int, int
// closedcard:int, int
bool token_playerpos(strvector::const_iterator& i,
                     strvector::const_iterator& end,
                     CTableView* pView)
{
  ++i;
  if (i == end)
    return error(pView, "Invalid playerpos: slot number expected");

  string s = *i;
  lowercase(s);

  int slot = 0;  
  if (s == string("deck"))
    slot = PI_House;
  else if (s == string("community"))
    slot = PI_Community;
  else
    slot = atoi(s.c_str());

  ++i;
  if (i == end)
    return error(pView, "Invalid player token");

  if (!ReadPos(i, end, s))
    return error(pView, "Invalid playerpos keyword");
  pView->setPlayerLabelPos(slot, MakePoint(s));

  if (!ReadPos(i, end, s))
    return error(pView, "Invalid playerpos keyword");
  pView->setPlayerOpenCardPos(slot, MakePoint(s));

  if (!ReadPos(i, end, s))
    return error(pView, "Invalid playerpos keyword");
  pView->setPlayerBacksidePos(slot, MakePoint(s));

  --i;

  return true;
}

// token:     'chippos'
// slot:      int 
// chippos:   int, int
// anchor:    int (0 => left, 1 => right)
bool token_chippos(strvector::const_iterator& i,
                   strvector::const_iterator& end,
                    CTableView* pView)
{
  ++i;
  if (i == end)
    return error(pView, "Invalid chippos: slot number expected");

  string s = *i;
  lowercase(s);

  int slot = 0;  
  if (s == string("deck"))
    slot = PI_House;
  else if (s == string("community"))
    slot = PI_Community;
  else
    slot = atoi(s.c_str());

  ++i;
  if (i == end)
    return error(pView, "Invalid chippos token");

  if (!ReadPos(i, end, s))
    return error(pView, "Invalid chippos keyword");
  CPoint pos = MakePoint(s);

  if (i == end)
    return error(pView, "Invalid chippos: anchor value (0, 1) expected");

  int anchor = atoi((*i).c_str());

  pView->setPlayerChipPos(slot, pos, anchor);

  return true;
}


// token:     'turnpointer'
// slot:      int 
// direction: 'top', 'bottom', 'left', 'right'
bool token_turnpointer(strvector::const_iterator& i,
                       strvector::const_iterator& end,
                       CTableView* pView)
{
  ++i;
  if (i == end)
    return error(pView, "Invalid turnpointer: slot number expected");

  string s = *i;
  lowercase(s);

  int slot = 0;  
  if (s == string("deck"))
    slot = PI_House;
  else if (s == string("community"))
    slot = PI_Community;
  else
    slot = atoi(s.c_str());

  ++i;
  if (i == end)
    return error(pView, "Invalid turnpointer: direction expected");

  s = *i;
  lowercase(s);
  
  if (s == string("left"))
    pView->setPlayerTurnindicatorDir(slot, PlayerSlot::TID_Left);
  else if (s == string("right"))
    pView->setPlayerTurnindicatorDir(slot, PlayerSlot::TID_Right);
  else if (s == string("top"))
    pView->setPlayerTurnindicatorDir(slot, PlayerSlot::TID_Top);
  else if (s == string("bottom"))
    pView->setPlayerTurnindicatorDir(slot, PlayerSlot::TID_Bottom);
  else
    return error(pView, "Invalid turnpointer: direction must be one of: 'top', 'bottom', 'left', 'right'");    

  return true;
}

// token:     'label'
// background color:
// red:       BYTE
// green:     BYTE
// blue:      BYTE
// alpha:     BYTE
bool token_label(strvector::const_iterator& i,
                 strvector::const_iterator& end,
                 CTableView* pView)
{
  ++i;
  if (i == end)
    return error(pView, "Invalid label: red color component expected");

  BYTE r = atoi((*i).c_str());

  ++i;
  if (i == end)
    return error(pView, "Invalid label: green color component expected");

  BYTE g = atoi((*i).c_str());

  ++i;
  if (i == end)
    return error(pView, "Invalid label: blue color component expected");

  BYTE b = atoi((*i).c_str());

  ++i;
  if (i == end)
    return error(pView, "Invalid label: alpha color component expected");

  BYTE a = atoi((*i).c_str());

  Player::SetLabelAlpha(r, g, b, a);

  return true;
}


// token:     'background'
// filepath:  string
bool token_background(strvector::const_iterator& i,
                      strvector::const_iterator& end,
                      CTableView* pView)
{
  ++i;
  if (i == end)
    return error(pView, "Invalid background: image file path expected");

  string path = *i;
  pView->loadBackground(path.c_str(), true);

  return true;
}

// token:     'cardopen'
// image filepath:  string
// mask filepath:   string
bool token_cardimage(strvector::const_iterator& i,
                     strvector::const_iterator& end,
                     CTableView* pView)
{
  ++i;
  if (i == end)
    return error(pView, "Invalid cardopen: image file path expected");

  string imagepath = *i;

  ++i;
  if (i == end)
    return error(pView, "Invalid cardopen: mask file path expected");

  string maskpath = *i;

  CString errMsg;
  if (!Cards::Instance().loadOpenCardBitmaps(imagepath.c_str(),
                                             maskpath.c_str(),
                                             &errMsg))
  {
    CString msg;
    msg.Format("Error loading open card bitmaps:\n\n%s", (LPCTSTR)errMsg);
    pView->MessageBox((LPCTSTR)msg);
  }

  return true;
}

// token:     'cardbackside'
// image filepath:  string
// mask filepath:   string
bool token_cardbackside(strvector::const_iterator& i,
                        strvector::const_iterator& end,
                        CTableView* pView)
{
  ++i;
  if (i == end)
    return error(pView, "Invalid cardbackside: image file path expected");

  string imagepath = *i;

  ++i;
  if (i == end)
    return error(pView, "Invalid cardbackside: mask file path expected");

  string maskpath = *i;

  CString errMsg;
  if (!Cards::Instance().loadBacksideBitmaps(imagepath.c_str(),
                                             maskpath.c_str(),
                                             &errMsg))
  {
    CString msg;
    msg.Format("Error loading closed card bitmaps:\n\n%s", (LPCTSTR)errMsg);
    pView->MessageBox((LPCTSTR)msg);
  }

  return true;
}


// token:     'text'
// text color:
// red:       BYTE
// green:     BYTE
// blue:      BYTE
bool token_text(strvector::const_iterator& i,
                strvector::const_iterator& end,
                CTableView* pView)
{
  ++i;
  if (i == end)
    return error(pView, "Invalid text: red text color component expected");

  BYTE r = atoi((*i).c_str());

  ++i;
  if (i == end)
    return error(pView, "Invalid text: green text color component expected");

  BYTE g = atoi((*i).c_str());

  ++i;
  if (i == end)
    return error(pView, "Invalid text: blue text color component expected");

  BYTE b = atoi((*i).c_str());

  Player::SetTextColor(r, g, b);

  return true;
}

// token:     'bumptext'
// bump text status:
// on:  'on'
// off: 'off'
bool token_bumptext(strvector::const_iterator& i,
                    strvector::const_iterator& end,
                    CTableView* pView)
{
  ++i;
  if (i == end)
    return error(pView, "Invalid bump text: status 'on' or 'off' expected");

  string s = (*i);
  lowercase(s);

  if (s == "on")
    Player::bump_text_ = TRUE;
  else
    Player::bump_text_ = FALSE;

  return true;
}


bool token_localplayer(strvector::const_iterator& i,
                       strvector::const_iterator& end,
                       CTableView* pView)
{
  ++i;
  if (i == end)
    return error(pView, "Invalid localplayer token");

  int slot = atoi((*i).c_str());

  if (slot >= 0 && slot < CTableView::MaxPlayers)
  {
    pView->setLocalPlayer(pView->getPlayer(slot));
  }

  return true;
}


bool token_unknown(strvector::const_iterator& i,
                   strvector::const_iterator& end,
                   CTableView* pView)
{
  CString msg;
  msg.Format("Unknown keyword: %s", (*i).c_str());
  error(pView, msg);
  return false;
}


// token:     'chatcolor'
// type:      'user', 'normal' 'critical'
// text color:
// red:       BYTE
// green:     BYTE
// blue:      BYTE
bool token_chatcolor(strvector::const_iterator& i,
                     strvector::const_iterator& end,
                     CTableView* pView)
{
  ++i;
  if (i == end)
    return error(pView, "Invalid chatcolor: 'user', 'normal', 'critical' expected");

  string s = *i;
  lowercase(s);

  if (!(s == string("user") ||
        s == string("normal") ||
        s == string("critical")))
    return error(pView, "Invalid chatcolor: 'user', 'normal', 'critical' expected");

  ++i;
  if (i == end)
    return error(pView, "Invalid chatcolor: red text color component expected");

  BYTE r = atoi((*i).c_str());

  ++i;
  if (i == end)
    return error(pView, "Invalid chatcolor: green text color component expected");

  BYTE g = atoi((*i).c_str());

  ++i;
  if (i == end)
    return error(pView, "Invalid chatcolor: blue text color component expected");

  BYTE b = atoi((*i).c_str());

  if (s == string("user"))
    CChatView::SetUserColor(RGB(r, g, b));
  else if (s == string("normal"))
    CChatView::SetNormalColor(RGB(r, g, b));
  else if (s == string("critical"))
    CChatView::SetCriticalColor(RGB(r, g, b));

  return true;
}


// token:     'announcement'
// rect:      left, top, right, bottom
bool token_announcement(strvector::const_iterator& i,
                        strvector::const_iterator& end,
                        CTableView* pView)
{
  ++i;
  if (i == end)
    return error(pView, "Invalid announcement: rect coordinates expected");

  int left = atoi((*i).c_str());

  ++i;
  if (i == end)
    return error(pView, "Invalid announcement: rect coordinates expected");

  int top  = atoi((*i).c_str());

  ++i;
  if (i == end)
    return error(pView, "Invalid announcement: rect coordinates expected");

  int right = atoi((*i).c_str());

  ++i;
  if (i == end)
    return error(pView, "Invalid announcement: rect coordinates expected");

  int bottom = atoi((*i).c_str());

  CTableView::SetAnnouncementRect(CRect(left, top, right, bottom));

  return true;
}


struct TokenHandler
{
  string name_;
  token_handler pHandler_;

  TokenHandler(const string& s, token_handler p)
    : name_(s), pHandler_(p)
  {}
};

TokenHandler g_TokenHandlers[] = 
{
  { TokenHandler(string("/*"), token_comment) },
  { TokenHandler(string("playerpos"), token_playerpos) },
  { TokenHandler(string("chippos"), token_chippos) },
  { TokenHandler(string("label"), token_label) },
  { TokenHandler(string("background"), token_background) },
  { TokenHandler(string("text"), token_text) },
  { TokenHandler(string("bumptext"), token_bumptext) },
  { TokenHandler(string("chatcolor"), token_chatcolor) },
  { TokenHandler(string("localplayer"), token_localplayer) },
  { TokenHandler(string("cardimage"), token_cardimage) },
  { TokenHandler(string("cardbackside"), token_cardbackside) },
  { TokenHandler(string("turnpointer"), token_turnpointer) },
  { TokenHandler(string("announcement"), token_announcement) }
};


token_handler find_handler(string token)
{
  lowercase(token);

  for (int i = 0;
       i < (sizeof(g_TokenHandlers) / sizeof(g_TokenHandlers[0]));
       i++)
  {
    if (g_TokenHandlers[i].name_ == token)
      return g_TokenHandlers[i].pHandler_;
  }

  return token_unknown;
}


void interpret(strvector& data, CTableView* pView)
{
  bool ok = true;

  for (strvector::const_iterator i = data.begin(),
                                 end = data.end();
       i != end && ok;
       ++i)
  {
    string s = *i;
    token_handler handler = find_handler(s);
    if (handler)
      ok = (*handler)(i, end, pView);
  }
}



//
// Load configuration from file 'fname'.
//
void LoadLayout(const char* fname, CTableView* pView)
{  
  strvector data;
  ifstream ifs(fname);

  if (!ifs.is_open())
  {
    CString s;
    s.Format("Can't open layout data file: %s - will use default layout.", fname);
    pView->MessageBox(s, NULL, MB_OK|MB_ICONEXCLAMATION);
    ifs.open("default.txt");
  }

  if (!ifs.is_open())
  {
    CString s;
    s.Format("The layout file 'default.txt' is missing.");
    pView->MessageBox(s);
  }

  if (ifs.is_open())
  {
    // Read the file contents
    copy(string_input(ifs), string_input(), back_inserter(data));

    if (data.size() < 2)
      return;

    interpret(data, pView);
  }
}


}

