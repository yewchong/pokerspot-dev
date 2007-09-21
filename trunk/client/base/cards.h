#ifndef __base_cards_h__
#define __base_cards_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class Base::Cards
  Created  : 25.10.1999

  OVERVIEW : Class for drawing & animating cards.

 ****************************************************************************/

namespace Base
{

class Card;

class Cards
{
public:

  enum {
    C_Margin  = 2
  };

  enum {
    C_BackSide = 1
  };

  static int Width_, Height_, XOffset_, YOffset_;
  
  static int OpenCardWidth_, OpenCardHeight_;
  static int BacksideCardWidth_, BacksideCardHeight_;

  BOOL loadOpenCardBitmaps(LPCTSTR pszCardImageFile,
                           LPCTSTR pszMask,
                           CString* pErrMsg = NULL);
  BOOL loadOpenCardBitmaps2(LPCTSTR pszCardImageFile,
                            LPCTSTR pszMask,
                            CString* pErrMsg = NULL);
  BOOL loadBacksideBitmaps(LPCTSTR pszBackSideFile,
                           LPCTSTR pszMask,
                           CString* pErrMsg = NULL);

public:
  Cards();
  virtual ~Cards();

public:
  static Cards& Instance();

  BOOL drawCard(CDC* pDC, const Card& card, int xpos, int ypos, DWORD dwFlags = 0);
  BOOL animate(CDC* pDC, const Card& card, const CPoint& newPos, const CPoint& oldPos, DWORD dwFlags = C_BackSide);
  BOOL startAnimate(CDC* pDC, const CPoint& pos, DWORD dwFlags = C_BackSide);
  BOOL stopAnimate(CDC* pDC, const CPoint& pos, DWORD dwFlags = C_BackSide);

  BOOL animate(CDC* pDC, const CBitmap& bmpImage, const CBitmap& bmpMask,
               int width, int height, const CPoint& newPos, const CPoint& oldPos);
  BOOL startAnimate(CDC* pDC, const CBitmap& bmpImage, const CBitmap& bmpMask,
                    int width, int height, const CPoint& pos);
  BOOL stopAnimate(CDC* pDC, int width, int height, const CPoint& pos);

  int  startFlip(const Card& card, const CPoint& pos);
  BOOL flip(CDC* pDC, const Card& card, const CPoint& pos, DWORD dwFlags = 0);

private:
  CBitmap& getImage(const Card& card);
  BOOL bltCard(CDC* pDCTo, 
               int xpos, int ypos,
               int width, int height,
               int cardx, int cardy,
               CBitmap* pBmpImage,
               CBitmap* pBmpMask);
  BOOL loadBitmaps();

  CBitmap bmpCards_,
          bmpCardsMask_,
          bmpBuf_;
  CBitmap bmpCardImage_,
          bmpFaceDown_;
  CBitmap bmpBack_,
          bmpBackMask_;
  CBitmap bmpSave_;
  CBitmap bmpInvalid_;

public:
  static BOOL DrawSprite(HDC hdc,
                         const CPoint& newPos,
                         const CPoint& oldPos,
                         int width, int height,
                         HBITMAP mHSave,
                         HBITMAP mHImage,
                         HBITMAP mHMask);
};

} // Base

#endif
