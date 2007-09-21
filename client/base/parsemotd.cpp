#include "stdafx.h"
#include "base/parsemotd.h"
#include "base/misc.h"
#include "ui/global.h"
#include <stack>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace Base
{

struct RenderContext;
void RenderMotd(CDC* pDC, CString text, const CRect& r);
void Emit(const CString& s, RenderContext*);
BOOL ExecuteTag(const CString& tag, RenderContext*);


CRect SizeAnnounceText(CDC* pDC, const CString& text, const CRect& rect)
{
  CRect r(rect);
  r.InflateRect(-2, -4);
  pDC->DrawText(text, text.GetLength(), &r, DT_CALCRECT|DT_WORDBREAK);
  return r;
}

void DrawAnnounceText(CDC* pDC, const CString& text, const CRect& rect)
{
  CRect r(rect);
  r.InflateRect(-2, -4);
  pDC->DrawText(text, text.GetLength(), &r, DT_WORDBREAK);
//  RenderMotd(pDC, text, r);
}



const char TagStart = '<';
const char TagEnd   = '>';
const char StopTag  = '/';

struct RenderContext
{
  CDC*  pDC;
  CRect rect;
  int   left, right;
  UINT  flags;
  CFont fontBold, fontItalic, fontUnderline;
  std::stack<CFont*> fonts;

  RenderContext()
    : pDC(0), rect(0, 0, 0, 0),
      left(0), right(0), flags(0)
  {
    CFont* pFont = Global::GetFont();
    LOGFONT lf;
    pFont->GetLogFont(&lf);

    LONG w = lf.lfWeight;
    lf.lfWeight = FW_BOLD;
    fontBold.CreateFontIndirect(&lf);
    lf.lfWeight = w;

    BYTE i = lf.lfItalic;
    lf.lfItalic = TRUE;
    fontItalic.CreateFontIndirect(&lf);
    lf.lfItalic = i;

    BYTE u = lf.lfUnderline;
    lf.lfUnderline = TRUE;
    fontUnderline.CreateFontIndirect(&lf);
    lf.lfUnderline = u;

    // Set up the default font
    fonts.push(pFont);
  }
} g_RenderContext;


void RenderMotd(CDC* pDC, CString text, const CRect& r)
{
  g_RenderContext.pDC = pDC;
  g_RenderContext.rect = r;
  g_RenderContext.left = r.left;
  g_RenderContext.right = r.right;
  g_RenderContext.flags = 0;

  CString s, tag;
  
  // Copy characters to s until tag found
  int len = text.GetLength(),
      pos = 0;
  bool done = false;  
  
  while (!done)
  {
    int i = text.Find(TagStart, pos);
    if (i == -1)
    { // No tags, emit text and we're done
      Emit(text, &g_RenderContext);
      done = true;
    }
    else
    { // Possible tag
      s = text.Left(i);

      if (s.GetLength() > 0)
      { // Emit text left of tag
        Emit(s, &g_RenderContext);
        // and drop it
        text = text.Right(text.GetLength() - i);
      }

      pos = 0;
      i = text.Find(TagEnd, pos + 1);

      if (i != -1)
      { // Tag found - apply it
        tag = text.Mid(pos + 1, i - 1);

        if (ExecuteTag(tag, &g_RenderContext))
        { // it was a valid tag, don't emit it
          text = text.Right(text.GetLength() - i - 1);
        }
        else
        { // Unknown tag!
          text = text.Right(text.GetLength() - i - 1);
        }
      }
    }
  }
}


struct Tag
{
  virtual void begin(RenderContext*) {};
  virtual void end(RenderContext*) {};
  virtual ~Tag() {};
};

struct TagCenter : public Tag
{
  void begin(RenderContext* pRC) { old = pRC->flags; pRC->flags |= DT_CENTER; }
  void end(RenderContext* pRC) { pRC->flags = old; }
  static TagCenter* Create() { return new TagCenter(); }

  UINT old;
};

struct TagItalic : public Tag
{
  void begin(RenderContext*) {};
  void end(RenderContext*) {};
  static TagItalic* Create() { return new TagItalic(); }
};

struct TagBold : public Tag
{
  void begin(RenderContext*) {};
  void end(RenderContext*) {};
  static TagBold* Create() { return new TagBold(); }
};

struct TagUnderline : public Tag
{
  void begin(RenderContext*) {};
  void end(RenderContext*) {};
  static TagUnderline* Create() { return new TagUnderline(); }
};

struct TagRule : public Tag
{
  void begin(RenderContext*) {};
  void end(RenderContext*) {}; 
  static TagRule* Create() { return new TagRule(); }
};

struct TagNewline : public Tag
{
  void begin(RenderContext* pRC)
  {
    pRC->rect.OffsetRect(0, Global::GetCharHeight());
    pRC->rect.left = pRC->left;
    pRC->right = pRC->right;
  };
  static TagNewline* Create() { return new TagNewline(); }
};

struct TagAssoc
{
  CString tagname;
  Tag*    pTag;

  TagAssoc(const CString& t, Tag* p)
    : tagname(t), pTag(p)
  {}
};



TagAssoc tagAssocs[] =
{
  { TagAssoc(CString("center"), TagCenter::Create()) },
  { TagAssoc(CString("bold"), TagBold::Create()) },
  { TagAssoc(CString("underline"), TagUnderline::Create()) },
  { TagAssoc(CString("rule"), TagRule::Create()) },
  { TagAssoc(CString("br"), TagNewline::Create()) }
};


void EmitWord(const CString& s, RenderContext* pRC)
{
  if (s.GetLength() == 0) return;

  const int space = 5;

  CRect r(pRC->rect);
  pRC->pDC->DrawText(s, &r, DT_CALCRECT);
  pRC->pDC->DrawText(s, &pRC->rect, pRC->flags);

  // maintain current rectangle
  if (pRC->flags & DT_CENTER)
  { 
    CenterRect(r, pRC->rect, DT_CENTER);
    pRC->rect.left = r.right + space;
  }
  else
  {
    r.OffsetRect(r.Width() + space, 0);
    pRC->rect.left = r.right;
  }

  if (r.right >= pRC->right)
  { // newline
    pRC->rect.OffsetRect(0, r.Height());
    pRC->rect.left = pRC->left;
    pRC->right = pRC->right;
  }
}


void Emit(const CString& text, RenderContext* pRC)
{
  EmitWord(text, pRC);
  return;

/*
  // Split string to tokens separated by
  // spaces and emit each token
  int length = text.GetLength();
  char* string = new char[length + 1];
  if (!string) return;

  strncpy(string, (LPCSTR)text, length);
  string[length] = '\0';
  const char* seps = " \n";
  char* token = strtok(string, seps);

  while (token != NULL)
  {
    EmitWord(token, pRC);
    token = strtok(NULL, seps);
  }

  delete [] string;
*/
}


BOOL ExecuteTag(const CString& tag, RenderContext* pRC)
{
  BOOL rc = FALSE;

  CString t(tag);
  BOOL tagEnd = (t[0] == StopTag);
  if (tagEnd)
    t = t.Right(t.GetLength() - 1);

  for (int i = 0;
       i < sizeof(tagAssocs) / sizeof(tagAssocs[0]);
       i++)
  {
    if (tagAssocs[i].tagname == t)
    {
      rc = TRUE;
      if (tagEnd)
        tagAssocs[i].pTag->end(pRC);
      else
        tagAssocs[i].pTag->begin(pRC);
      break;
    }
  }

  return rc;
}

          
}