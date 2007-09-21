/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class SoundManager
  Created  : 26.10.1999

  OVERVIEW : SoundManager is a simple class that allows
             playing wavs.

 ****************************************************************************/

#include "stdafx.h"
#include "soundmanager.h"
#include "registry.h"
#include "ui/resource.h"
#include <mmsystem.h>

namespace
{
#ifdef PSPOT_TABLE_MODULE_
  const char* g_SoundRes[SoundManager::SM_LastSound];
  SoundManager g_SoundManager;
#endif
}


SoundManager* SoundManager::pInst_ = 0;


SoundManager::SoundManager()
  :
  enabled_(TRUE)
{
  pInst_ = this;

#ifdef PSPOT_TABLE_MODULE_ // only table has sounds
  ::ZeroMemory(g_SoundRes, sizeof(g_SoundRes));
  enabled_ = Registry::GetSoundEnabled(enabled_);
#else
  enabled_ = FALSE;
#endif

#ifdef PSPOT_TABLE_MODULE_
  g_SoundRes[SM_Shuffle]  = "shuffle.wav";
  g_SoundRes[SM_Shuffle2]  = "shuffle2.wav";
  g_SoundRes[SM_Shuffle3]  = "shuffle.wav";
  g_SoundRes[SM_Card]     = "card.wav";
  g_SoundRes[SM_Card2]    = "card2.wav";
  g_SoundRes[SM_Card3]    = "card3.wav";
// No check sound, too disturbing!
//  g_SoundRes[SM_Check]    = "check.wav";
  g_SoundRes[SM_Call]     = "call.wav";
  g_SoundRes[SM_Raise]    = "raise.wav";
  g_SoundRes[SM_Fold]     = "fold.wav";
  g_SoundRes[SM_YourTurn] = "your_turn.wav";
  g_SoundRes[SM_WakeUp]   = "wake_up.wav";
  g_SoundRes[SM_Show]     = "show.wav";
  g_SoundRes[SM_RakeInSmall] = "rakes.wav";
  g_SoundRes[SM_RakeInLarge] = "rakel.wav";
  g_SoundRes[SM_Tournament] = "fanfar1.wav";
#endif
}


SoundManager::~SoundManager()
{
  pInst_ = 0;
}


BOOL SoundManager::playSound(Sound s, DWORD flags)
{
#ifdef PSPOT_TABLE_MODULE_
  if (enabled_ && g_SoundRes[s] != 0)
  {
    s = randomize(s);

    BOOL rc = ::PlaySound(g_SoundRes[s], NULL,
                          SND_ASYNC|SND_FILENAME);

    return rc;
  }
  else
#endif
    return FALSE;
}


SoundManager::Sound
SoundManager::randomize(SoundManager::Sound s) const
{
  if (s == SM_Card2)
  { // Choose another card sound every now and then
    int r = (rand() % 4);
    if (r == 0)
      s = SM_Card3;
    else if (r == 1)
      s = SM_Card;
  }
  else if (s == SM_Shuffle)
  {
    int r = (rand() % 5);
    if (r == 0)
      s = SM_Shuffle2;
    else if (r == 1)
      s = SM_Shuffle3;
  }

  return s;
}
