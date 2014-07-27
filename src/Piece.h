#ifndef PIECE_H
#define PIECE_H

extern "C"
{
        #include <os.h>
        #include "SDL/SDL.h"
}

#include "Metrics.h"
#include "Config.h"
#include "Selector.h"
#include "MovesRemainingLabel.h"

struct PieceSprite
{
   SDL_Surface* img;
   SDL_Surface* replace;
   int nPieceType;
   int boardX, boardY;
   int x,y;
   int toX, toY;
   bool toFade;
   int nFadeAmount;
   struct PieceSprite* next;
};

class PieceControl
{
public:
   PieceControl(SDL_Surface* pScreen, Metrics* pBoardMetrics, Config* pConfig, Selector* pSelector, MovesRemainingLabel* pLabel);
   ~PieceControl();

   void ClearPieces();
   bool CreatePiece(int nType, int nX, int nY);
   bool SwapPieces(int nX1, int nY1, int nX2, int nY2);
   bool DropPiece(int nX, int nY);
   bool DisappearPiece(int nX, int nY);
   bool IsAnimating() const;
   bool Animate();

protected:
   SDL_Surface* m_pScreen;//Does NOT own
   Metrics* m_pBoardMetrics;//Does NOT own
   Config* m_pConfig;//Does NOT own
   Selector* m_pSelector;//Does NOT own
   MovesRemainingLabel* m_pMovesLabel;//Does NOT own
   PieceSprite* m_pRoot;
};

#endif


