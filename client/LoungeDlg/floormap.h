#ifndef __loungedlg_floormap_h__
#define __loungedlg_floormap_h__

#include "network/network.h"

class CChips;

// Special CFloorMapView class for the
// dialog-based Lounge client.
// This class is not really a view but
// instead forwards everything to the
// actual CLoungeDlg.

class CFloorMap
{
public:
  CFloorMap();

  static CFloorMap* Instance();

  void Invalidate();
  void disconnect();

  void setStats(int players, int tables);
  void addGame(u_int16_t gametype, const string& name);
  void addQueue(u_int16_t gametype, u_int16_t number,
                u_int16_t players,
                const CChips& lolimit, const CChips& hilimit);
  void recalcLayout();
  void setTitle();
  void getTableLimits(USHORT number,
                      CChips& lolimit,
                      CChips& hilimit,
                      USHORT& isHiLo,
                      USHORT& gameType);

  void joinedTable(USHORT number, const CChips& lo, const CChips& hi, ULONG launch_id);
  void launchFailed(USHORT number, const CChips& lo, const CChips& hi);
};


#endif