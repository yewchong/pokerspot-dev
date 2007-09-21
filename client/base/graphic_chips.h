#ifndef __base_chips_h__
#define __base_chips_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class Chips
  Created  : 03/03/2000

  OVERVIEW : Class for drawing & animating chips.

 ****************************************************************************/


#include "common/chips.h"

namespace Base
{

class Chips
{
public:

  static int Width_, Height_;
  
  BOOL loadChipGraphics(LPCTSTR pszFileBase, CString* pErrMsg = NULL);

public:
  Chips();
  virtual ~Chips();

public:
  static Chips& Instance();

  enum DrawFlag
  {
    DF_CreateImage = 1,
    DF_CreateMask  = 2
  };

  BOOL drawChips(CDC* pDC, const CChips& chips, int xpos, int ypos, DWORD dwFlags = 0);
  CSize getChipsSize(const CChips& chips, int xpos, int ypos);
  int   getNumChipUnits(const CChips& chips);
  BOOL animate(CDC* pDC, const CChips& chips, const CPoint& newPos, const CPoint& oldPos);
  BOOL startAnimate(CDC* pDC, const CChips& chips, const CPoint& pos, DWORD dwFlags = 0);
  BOOL stopAnimate(CDC* pDC, const CPoint& pos, DWORD dwFlags = 0);
  CSize getAnimationBufferSize();

private:
  // Blit chips as one stack
  BOOL bltChips(CDC* pDCTo, 
                int xpos, int ypos,
                int width, int height,
                int x, int y,
                CDC& bufDC,
                CDC& memDC,
                CDC& transDC);
  // Blit each chip separately
  BOOL bltChips(CDC* pDCTo, 
                const CChips& chips,
                UINT chip_unit,
                CBitmap* pImage,
                int xpos, int ypos,
                int width, int height,
                int& next_y, // returns ypos of next chip
                CDC& bufDC,
                CDC& memDC,
                CDC& transDC);

  BOOL bltChips(CDC* pDCTo, 
                const CChips& chips,
                UINT chip_unit,
                CBitmap* pImage,
                CBitmap* pMask,
                int xpos, int ypos,
                int width, int height,
                int  chipsPerStack,
                int& cps,
                int& next_x, // returns xpos of next chip
                int& next_y, // returns ypos of next chip
                CDC& bufDC,
                CDC& memDC,
                CDC& transDC);

  void sizeChips(const CChips& chips,
                 UINT chip_unit,
                 int xpos, int ypos,
                 int width, int height,
                 int  chipsPerStack,
                 int& cps,
                 int& next_x, // returns xpos of next chip
                 int& next_y, // returns ypos of next chip
                 int& top_y); // returns ypos of highest chip



  CBitmap bmpBuf_, bmpSave_;

  // For animating a pile of chips:
  CBitmap chipsImage_, chipsMask_;
  BOOL createAnimationBuffers(const CChips& chips);
  CChips chips_;

  CBitmap oneChip_,
          fiveChip_,
          tenChip_,
          twenty5Chip_,
          fiftyChip_,
          hundredChip_,
          fiveHundredChip_,
          thousandChip_,
		  fiveThousandChip_,
          chipMask_,
          chipMask2_;
  bool drawChips_;
};

} // Base

#endif
