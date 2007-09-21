/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class ListCtrl
  Created  : 02/03/2000

  OVERVIEW : ListCtrl is an owner-draw subclass of CListCtrl.

 ****************************************************************************/

#include "stdafx.h"
#include "listctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


int g_cyItem = 0;

namespace
{

void DrawItemColumn(HDC hdc, LPCSTR lpsz, LPRECT prcClip);
BOOL CalcStringEllipsis(HDC hdc, LPTSTR lpszString, int cchMax, UINT uColWidth);

//
//  FUNCTION:   DrawListViewItem(const LPDRAWITEMSTRUCT)
//
//  PURPOSE:    Draws one item in the listview control.
//
//  PARAMETERS:
//      lpDrawItem - Pointer to the information needed to draw the item.  The
//                   item number is in the itemID member.
//
void DrawListViewItem(ListCtrl* pList, LPDRAWITEMSTRUCT lpDrawItem,
                      int sel_row, int sel_col)
{
    HIMAGELIST himl;
    LV_ITEM lvi;
    int cxImage = 0, cyImage = 0;
    UINT uFirstColWidth;
    RECT rcClip;
    int iColumn = 1;
	UINT uiFlags = ILD_TRANSPARENT;


    // Get the item to be displayed
    lvi.mask = LVIF_IMAGE | LVIF_STATE;
    lvi.iItem = lpDrawItem->itemID;
    lvi.iSubItem = 0;
    ListView_GetItem(lpDrawItem->hwndItem, &lvi);

    // Check to see if this item is selected
#ifdef LISTCTRL_SELECTION_
    if (lpDrawItem->itemState & ODS_SELECTED)
    {
        // Set the text background and foreground colors
        SetTextColor(lpDrawItem->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
        SetBkColor(lpDrawItem->hDC, GetSysColor(COLOR_HIGHLIGHT));

	    // Also add the ILD_BLEND50 so the images come out selected
	    uiFlags |= ILD_BLEND50;
    }
    else
#endif
    {
        // Set the text background and foreground colors to the standard window
        // colors
        SetTextColor(lpDrawItem->hDC, GetSysColor(COLOR_WINDOWTEXT));
        SetBkColor(lpDrawItem->hDC, GetSysColor(COLOR_WINDOW));
    }


    // Get the image list and draw the image
    himl = ListView_GetImageList(lpDrawItem->hwndItem, LVSIL_SMALL);
    if (himl)
    {
        ImageList_Draw(himl, lvi.iImage, lpDrawItem->hDC,
                       lpDrawItem->rcItem.left, lpDrawItem->rcItem.top,
                       uiFlags);

        // Find out how big the image we just drew was
        ImageList_GetIconSize(himl, &cxImage, &cyImage);
    }

    // Calculate the width of the first column after the image width.  If
    // There was no image, then cxImage will be zero.
    int cxColumn = 0;
    pList->getColumnWidth(0, cxColumn);
    uFirstColWidth = cxColumn - cxImage;

    int columns = pList->GetHeaderCtrl()->GetItemCount();
    int i = 0;

    // Set up the new clipping rect for the first column text and draw it
    rcClip.left = lpDrawItem->rcItem.left + cxImage;
    rcClip.right = lpDrawItem->rcItem.left + cxColumn;
    rcClip.top = lpDrawItem->rcItem.top;
    rcClip.bottom = lpDrawItem->rcItem.bottom;

    // remember the item height
    g_cyItem = lpDrawItem->rcItem.bottom - lpDrawItem->rcItem.top;

    CString s;
    pList->getItemText(lpDrawItem->itemID, 0, s);    
    DrawItemColumn(lpDrawItem->hDC, s, &rcClip);

    for (i = 1; i < columns; i++)
    {
      // Update the clip rect to the column
      pList->getColumnWidth(i, cxColumn);
      rcClip.left = rcClip.right;
      rcClip.right = rcClip.left + cxColumn;
      pList->getItemText(lpDrawItem->itemID, i, s);
      DrawItemColumn(lpDrawItem->hDC, s, &rcClip);
    }

    // If we changed the colors for the selected item, undo it
    if (lpDrawItem->itemState & ODS_SELECTED)
    {
        // Set the text background and foreground colors
        SetTextColor(lpDrawItem->hDC, GetSysColor(COLOR_WINDOWTEXT));
        SetBkColor(lpDrawItem->hDC, GetSysColor(COLOR_WINDOW));
    }

    // If the item is focused, now draw a focus rect around the entire row
#ifdef LISTCTRL_FOCUS_
    if (lpDrawItem->itemState & ODS_FOCUS)
    {
        // Adjust the left edge to exclude the image
        rcClip = lpDrawItem->rcItem;
        rcClip.left += cxImage;

        // Draw the focus rect
        DrawFocusRect(lpDrawItem->hDC, &rcClip);
    }
#endif

    CRect rectArea;
    pList->GetClientRect(&rectArea);

    int xpos = 0;
    CDC* pDC = CDC::FromHandle(lpDrawItem->hDC);
    if (pDC)
    {
      CPen pen;
      pen.CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
      CPen* pOldPen = pDC->SelectObject(&pen);

      // Draw vertical column borders
      for (i = 1; i < columns; i++)
      {
        pList->getColumnWidth(i - 1, cxColumn);
        xpos += cxColumn;
        pDC->MoveTo(xpos - 1, lpDrawItem->rcItem.top);
        //pDC->LineTo(xpos - 1, lpDrawItem->rcItem.bottom);
        pDC->LineTo(xpos - 1, rectArea.bottom);
      }

      pDC->SelectObject(pOldPen);
    }

    if (sel_row != -1 && sel_col != -1)
      pList->hilite(sel_row, sel_col, lpDrawItem->itemState);

    return;
}


//
//  FUNCTION:   DrawItemColumn(HDC, LPCSTR, LPRECT)
//
//  PURPOSE:    Draws the text for one of the columns in the list view.
//
//  PARAMETERS:
//      hdc     - Handle of the DC to draw the text into.
//      lpsz    - String to draw.
//      prcClip - Rectangle to clip the string to.
//
void DrawItemColumn(HDC hdc, LPCSTR lpsz, LPRECT prcClip)
{
    TCHAR szString[256];

    // Check to see if the string fits in the clip rect.  If not, truncate
    // the string and add "...".
    lstrcpy(szString, lpsz);
    CalcStringEllipsis(hdc, szString, 256, prcClip->right - prcClip->left);

    // print the text
    ExtTextOut(hdc, prcClip->left + 2, prcClip->top + 1, ETO_CLIPPED | ETO_OPAQUE,
               prcClip, szString, lstrlen(szString), NULL);

}


//
//  FUNCTION:   CalcStringEllipsis(HDC, LPTSTR, int, UINT)
//
//  PURPOSE:    Determines whether the specified string is too wide to fit in
//              an allotted space, and if not truncates the string and adds some
//              points of ellipsis to the end of the string.
//
//  PARAMETERS:
//      hdc        - Handle of the DC the string will be drawn on.
//      lpszString - Pointer to the string to verify
//      cchMax     - Maximum size of the lpszString buffer.
//      uColWidth  - Width of the space in pixels to fit the string into.
//
//  RETURN VALUE:
//      Returns TRUE if the string needed to be truncated, or FALSE if it fit
//      into uColWidth.
//

BOOL CalcStringEllipsis(HDC hdc, LPTSTR lpszString, int cchMax, UINT uColWidth)
{
    const TCHAR szEllipsis[] = TEXT("...");
    SIZE   sizeString;
    SIZE   sizeEllipsis;
    int    cbString;
    LPTSTR lpszTemp;
    BOOL   fSuccess = FALSE;
    static BOOL fOnce = TRUE;

    // Assume GetTextExtentPoint32() API provided by the platform! (!Win32s)

    // Adjust the column width to take into account the edges
    uColWidth -= 4;

    __try
    {
        // Allocate a string for us to work with.  This way we can mangle the
        // string and still preserve the return value
        lpszTemp = (LPTSTR) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cchMax);
        if (!lpszTemp)
        {
            __leave;
        }
        lstrcpy(lpszTemp, lpszString);

        // Get the width of the string in pixels
        cbString = lstrlen(lpszTemp);
        if (!GetTextExtentPoint32(hdc, lpszTemp, cbString, &sizeString))
        {
            __leave;
        }

        // If the width of the string is greater than the column width shave
        // the string and add the ellipsis
        if ((ULONG)sizeString.cx > uColWidth)
        {
            if (!GetTextExtentPoint32(hdc, szEllipsis, lstrlen(szEllipsis),
                                       &sizeEllipsis))
            {
                __leave;
            }

            while (cbString > 0)
            {
                lpszTemp[--cbString] = 0;
                if (!GetTextExtentPoint32(hdc, lpszTemp, cbString, &sizeString))
                {
                    __leave;
                }

                if ((ULONG)(sizeString.cx + sizeEllipsis.cx) <= uColWidth)
                {
                    // The string with the ellipsis finally fits, now make sure
                    // there is enough room in the string for the ellipsis
                    if (cchMax >= (cbString + lstrlen(szEllipsis)))
                    {
                        // Concatenate the two strings and break out of the loop
                        lstrcat(lpszTemp, szEllipsis);
                        lstrcpy(lpszString, lpszTemp);
                        fSuccess = TRUE;
                        __leave;
                    }
                }
            }
        }
        else
        {
            // No need to do anything, everything fits great.
            fSuccess = TRUE;
        }
    }
    __finally
    {
        // Free the memory
        HeapFree(GetProcessHeap(), 0, (LPVOID)lpszTemp);
        return (fSuccess);
    }
}

}


ListCtrl::ListCtrl()
  :
  sel_row_(-1),
  sel_col_(-1)
{}
 

BOOL ListCtrl::getColumnWidth(int col, int& cx)
{
  BOOL rc = FALSE;
  CHeaderCtrl* pHeader = GetHeaderCtrl();

  if (pHeader)
  {
    HDITEM item;
    memset(&item, 0, sizeof(item));
    item.mask = HDI_WIDTH;
    rc = pHeader->GetItem(col, &item);
    if (rc)
      cx = item.cxy;
    else
      cx = 0;
  }

  return rc;
}


BOOL ListCtrl::getHeaderHeight(int& cy)
{
  BOOL rc = FALSE;
  CHeaderCtrl* pHeader = GetHeaderCtrl();

  if (pHeader)
  {
    rc = TRUE;
    CRect r(0, 0, 0, 0);
    pHeader->GetWindowRect(&r);
    cy = r.Height();
  }

  return rc;
}


BOOL ListCtrl::getItemText(int row, int col, CString& s)
{
  BOOL rc = FALSE;

  LV_ITEM item;
  char buf[256];
  memset(&item, 0, sizeof(item));
  item.mask = LVIF_TEXT;
  item.iItem = row;
  item.iSubItem = col;
  item.pszText = buf;
  item.cchTextMax = 255;

  rc = GetItem(&item);
  if (rc)
    s = item.pszText;
  else
    s = "";

  return rc;
}


BOOL ListCtrl::setItemText(int row, int col, const CString& s)
{
  if (GetItemCount() <= row)
  {
    InsertItem(row, 0, NULL);
  }
  return SetItemText(row, col, s);
}


void ListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItem)
{
  switch (lpDrawItem->itemAction)
  {
    // Just in case the implementation of the control changes in the
    // future, let's handle the other itemAction types too.
    case ODA_DRAWENTIRE:
//    case ODA_FOCUS:
//    case ODA_SELECT:
      DrawListViewItem(this, lpDrawItem, sel_row_, sel_col_);
      break;
    default:
        break;
  }

/*
  CRect rectArea;
  GetClientRect(&rectArea);

  int xpos = 0;
  CDC* pDC = CDC::FromHandle(lpDrawItem->hDC);
  if (pDC)
  {
    CPen pen;
    pen.CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
    CPen* pOldPen = pDC->SelectObject(&pen);

    // Draw vertical column borders
    for (int i = 1; i < columns; i++)
    {
      pList->getColumnWidth(i - 1, cxColumn);
      xpos += cxColumn;
      pDC->MoveTo(xpos - 1, lpDrawItem->rcItem.top);
      pDC->LineTo(xpos - 1, rectArea.bottom);
    }
  
    pDC->SelectObject(pOldPen);
  }
*/
}


void ListCtrl::push(int col, const CString& text)
{
  // Find first free slot
  for (int i = 0; ; i++)
  {
    CString s;    
    getItemText(i, col, s);
    if (s.IsEmpty())
    {
      setItemText(i, col, text);
      break;
    }
  }
}


void ListCtrl::hilite(int col, const CString& s)
{ // Not implemented
  ASSERT(FALSE);
}


CRect ListCtrl::hilite(int row, int col, DWORD flags)
{
  if (!(flags & ODS_SELECTED)) return CRect(0, 0, 0, 0);

  int scroll = 0;
  scroll = GetTopIndex() * g_cyItem;
  CPoint ptScroll(0, scroll);

  CRect r(0, 0, 0, 0);
  CString s;
  getItemText(row, col, s);

  char buf[100];
  sprintf(buf, "Hiliting: %s\n", s);
  OutputDebugString(buf);
  
  // Compute rectangle to highlite
  int cx = 0, xpos = 0;
  getColumnWidth(0, cx);
  for (int i = 0; i < col; i++)
  {
    getColumnWidth(i, cx);
    xpos += cx;
  }
  int header = 0, cy = g_cyItem;
  getHeaderHeight(header);

  CBrush br;
  br.CreateSolidBrush(RGB(0, 0, 200));
  CDC* pDC = GetDC();
  if (pDC)
  {
    //CPoint p = pDC->SetWindowOrg(ptScroll);

    int ypos = row * cy + header - scroll;
    r = CRect(xpos + 1, ypos, 
              xpos + cx - 2,
              ypos + g_cyItem);

    CFont* pOldFont = pDC->SelectObject(
      CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT)));

    CBrush br;
    br.CreateSolidBrush(::GetSysColor(COLOR_HIGHLIGHT));
    pDC->FillRect(&r, &br);

    pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
    int oldmode = pDC->SetBkMode(TRANSPARENT);
    pDC->DrawText(s, &r, DT_LEFT|DT_SINGLELINE|DT_VCENTER);

    pDC->SetBkMode(oldmode);
    pDC->SelectObject(pOldFont);
    //pDC->SetWindowOrg(p);

    ReleaseDC(pDC);
  }
  
  ClientToScreen(&r);
  return r;
}


void ListCtrl::remove(int col, const CString& text)
{
  // Find matching name and remove it
  // by nudging others up
  CString s;
  for (int i = 0; ; i++)
  {
    getItemText(i, col, s);
    if (s.IsEmpty() || s == text)
    {
      break;
    }
  }

  for (; !s.IsEmpty(); i++)
  {
    getItemText(i + 1, col, s);
    setItemText(i, col, s);
  }
}


void ListCtrl::getSelection(int& row, int& col)
{
  row = sel_row_;
  col = sel_col_;
}


void ListCtrl::setSelection(int row, int col)
{
  // Repaint old selection
  if (sel_row_ != -1 && sel_col_ != -1)
  {
    CRect r = getRect(sel_row_, sel_col_);
    InvalidateRect(&r);
  }

  CString s;
  getItemText(row, col, s);
  if (s.IsEmpty())
  {
    sel_row_ = -1;
    sel_col_ = -1;
  }
  else
  {  
    sel_row_ = row;
    sel_col_ = col;

    // Paint new selection
    if (sel_row_ != -1 && sel_col_ != -1)
    {
      CRect r = getRect(sel_row_, sel_col_);
      InvalidateRect(&r);
    }
  }
}


CRect ListCtrl::getRect(int row, int col)
{
  CRect r(0, 0, 0, 0);

  // Compute rectangle 
  int cx = 0, xpos = 0;
  getColumnWidth(0, cx);
  for (int i = 0; i < col; i++)
  {
    getColumnWidth(i, cx);
    xpos += cx;
  }
  int header = 0, cy = g_cyItem;
  getHeaderHeight(header);

  int scroll = 0;
  scroll = GetTopIndex() * g_cyItem;
  int ypos = row * cy + header - scroll;
  r = CRect(xpos + 1, ypos, 
            xpos + cx - 2,
            ypos + g_cyItem);

  return r;
}


void ListCtrl::drawItems(CDC* pDC)
{
    int columns = GetHeaderCtrl()->GetItemCount();
    CRect rectArea;
    GetClientRect(&rectArea);
    
    int xpos = 0;
    CPen pen;
    pen.CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
    CPen* pOldPen = pDC->SelectObject(&pen);
        
    // Draw vertical column borders
    int cxColumn = 0;
    for (int i = 1; i < columns; i++)
    {
        getColumnWidth(i - 1, cxColumn);
        xpos += cxColumn;
        pDC->MoveTo(xpos - 1, rectArea.top);
        pDC->LineTo(xpos - 1, rectArea.bottom);
    }
    pDC->SelectObject(pOldPen);
}

