#ifndef __base_table_h__
#define __base_table_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class Base::Table
  Created  : 29.10.1999

  OVERVIEW : Class table represents one table image in
             the floor map.

 ****************************************************************************/


#include <string>
#include "base/def.h"
#include "dib/dib.h"

namespace Base
{

class Table
{
public:
  Table(USHORT tableNumber, const CPoint& pos,
        LPCTSTR name,
        int players, int maxPlayers,
        USHORT lolimit, USHORT hilimit);
  virtual ~Table();

  virtual BOOL draw(CDC* pDC, DWORD dwFlags = 0) const;

  CPoint getPos() const { return pos_; }
  void setPos(const CPoint& p) { pos_ = p; }    
  CSize getSize() const { return Size_; }
  CRect getArea() const { return CRect(pos_, Size_); }

  USHORT number() const { return number_; }
  USHORT lolimit() const { return lolimit_; }
  USHORT hilimit() const { return hilimit_; }

  void  setPlayers(int p) { players_ = p; }

protected:
  USHORT  number_;
  CString name_;
  int     players_;
  int     maxPlayers_;
  USHORT  lolimit_;
  USHORT  hilimit_;

  CPoint          pos_;
  static  CSize   Size_;
  static  CBrush  Brush_;
  static  CDib    DibImage_;
};


} // Base

#endif