#ifndef MOVESREMAINING_H
#define MOVESREMAINING_H

extern "C"
{
        #include <os.h>
        #include "SDL/SDL.h"
	#include "PuzzleFrenzyLib/PuzzleFrenzyLib.h"
}

#include "Metrics.h"

class MovesRemainingLabel
{
public:
   MovesRemainingLabel(SDL_Surface* pScreen, Metrics* pMetrics);
   ~MovesRemainingLabel();

   void SetPuzzleFrenzyLib(PuzzleFrenzyLib* pPuzzleFrenzy);
  
   void DrawLabel();

protected:
   SDL_Surface	*m_pScreen;//Does not own
   Metrics      *m_pBoardMetrics;//Does NOT own
   nSDL_Font	*m_pFont;
   PuzzleFrenzyLib *m_pPuzzleFrenzy;
};

#endif

