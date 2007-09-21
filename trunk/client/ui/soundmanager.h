#pragma once
#ifndef __psi_soundmanager_h__
#define __psi_soundmanager_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class SoundManager
  Created  : 26.10.1999

  OVERVIEW : SoundManager is a simple class that allows
             playing wavs.

 ****************************************************************************/


class SoundManager
{
public:
  enum Sound
  {
    SM_Shuffle   = 0,
    SM_Card      = 1,
    SM_Check     = 2,
    SM_Call      = 3,
    SM_Raise     = 4,
    SM_Fold      = 5,
    SM_YourTurn  = 6,
    SM_WakeUp    = 7,
    SM_Card2     = 8,
    SM_Card3     = 9,
    SM_Show      = 10,
    SM_RakeInSmall = 11,
    SM_RakeInLarge = 12,
    SM_Shuffle2    = 13,
    SM_Shuffle3    = 14,
    SM_Tournament  = 15,
    SM_LastSound = 16
  };

public:
  SoundManager();
  ~SoundManager();
  static SoundManager* Instance() { return pInst_; }

  BOOL playSound(Sound, DWORD flags = 0);
  void enable(BOOL b) { enabled_ = b; }
  BOOL isEnabled() const { return enabled_; }

private:
  Sound randomize(Sound) const;
  BOOL enabled_;
  static SoundManager* pInst_;
};

typedef SoundManager SndMan;


#endif