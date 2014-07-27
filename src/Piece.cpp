#include "Piece.h"
#include "Defines.h"
#include "PieceGraphics.h"
#include "PieceGraphicsHiDef.h"
#include "PuzzleFrenzyLevels.h"//For the piece type defines

PieceControl::PieceControl(SDL_Surface* pScreen, Metrics* pBoardMetrics, Config* pConfig, Selector* pSelector, MovesRemainingLabel* pLabel)
: m_pScreen(pScreen), m_pBoardMetrics(pBoardMetrics), m_pConfig(pConfig), m_pSelector(pSelector), m_pMovesLabel(pLabel), m_pRoot(NULL)
{
}

PieceControl::~PieceControl()
{
   ClearPieces();
}

void PieceControl::ClearPieces()
{
   PieceSprite* pPiece = m_pRoot;
   while(pPiece != NULL) {
      PieceSprite* pTemp = pPiece;
      SDL_FreeSurface(pPiece->img);
      SDL_FreeSurface(pPiece->replace);
      pPiece = pPiece->next;
      free(pTemp);
   }
   m_pRoot = NULL;
}

bool PieceControl::CreatePiece(int nType, int nX, int nY)
{
   struct PieceSprite* pPiece = (PieceSprite*)malloc(sizeof(struct PieceSprite));

   pPiece->img = SDL_CreateRGBSurface(SDL_SWSURFACE, m_pBoardMetrics->GetPieceSize(), m_pBoardMetrics->GetPieceSize(), 16, 0, 0, 0, 0);

#ifndef PIECE_DRAW_COLOR
   SDL_Surface* pSurface;
   if( !m_pConfig->GetRetroStyle() ) {
      switch(nType) {
         default:
         case SMILE_VALUE:
#ifdef USE_BLACK_AND_WHITE_SMILE
            pSurface = nSDL_LoadImage(image_SmilePuzzleFrenzyHiDef);
#else
            pSurface = nSDL_LoadImage(image_ColorSmilePuzzleFrenzyHiDef);
#endif
         break;
         case SQUARES_VALUE:
            pSurface = nSDL_LoadImage(image_SquaresPuzzleFrenzyHiDef);
         break;
         case HASH_VALUE:
            pSurface = nSDL_LoadImage(image_HashPuzzleFrenzyHiDef);
         break;
         case BIG_SQUARE_VALUE:
            pSurface = nSDL_LoadImage(image_BigSquarePuzzleFrenzyHiDef);
         break;
         case BIG_CIRCLE_VALUE:
            pSurface = nSDL_LoadImage(image_BigCirclePuzzleFrenzyHiDef);
         break;
      }
   }
   else {
      switch(nType) {
         default:
         case SMILE_VALUE:
            pSurface = nSDL_LoadImage(image_SmilePuzzleFrenzy);
         break;
         case SQUARES_VALUE:
            pSurface = nSDL_LoadImage(image_SquaresPuzzleFrenzy);
         break;
         case HASH_VALUE:
            pSurface = nSDL_LoadImage(image_HashPuzzleFrenzy);
         break;
         case BIG_SQUARE_VALUE:
            pSurface = nSDL_LoadImage(image_BigSquarePuzzleFrenzy);
         break;
         case BIG_CIRCLE_VALUE:
            pSurface = nSDL_LoadImage(image_BigCirclePuzzleFrenzy);
         break;
      }
   }

   SDL_SoftStretch(pSurface, NULL, pPiece->img, NULL);
   SDL_FreeSurface(pSurface);

#else
   Uint32 clr;
   switch(nType) {
      default:
      case SMILE_VALUE:
         clr = SDL_MapRGB(m_pScreen->format, 0, 0, 255);
      break;
      case SQUARES_VALUE:
         clr = SDL_MapRGB(m_pScreen->format, 255, 0, 255);
      break;
      case HASH_VALUE:
         clr = SDL_MapRGB(m_pScreen->format, 0, 255, 255);
      break;
      case BIG_SQUARE_VALUE:
         clr = SDL_MapRGB(m_pScreen->format, 255, 255, 0);
      break;
      case BIG_CIRCLE_VALUE:
         clr = SDL_MapRGB(m_pScreen->format, 127, 127, 127);
      break;
   }

   SDL_FillRect(pPiece->img, NULL, clr);
#endif

   pPiece->replace = SDL_CreateRGBSurface(SDL_SWSURFACE, m_pBoardMetrics->GetPieceSize(), m_pBoardMetrics->GetPieceSize(), 16, 0, 0, 0, 0);

   pPiece->boardX = nX;
   pPiece->boardY = nY;
#ifdef PIECE_ANIMATE_RANDOM_START
   pPiece->x = rand() % 320;//I do random start position; I suppose it is alright :)
   pPiece->y = rand() % 240;
#else
   pPiece->x = m_pBoardMetrics->GetXPos(nX);
   pPiece->y = m_pBoardMetrics->GetYPos(nY);
#endif
   pPiece->toX = m_pBoardMetrics->GetXPos(nX);
   pPiece->toY = m_pBoardMetrics->GetYPos(nY);
   pPiece->toFade = false;
   pPiece->nFadeAmount = 0;

   pPiece->next = m_pRoot;
   m_pRoot = pPiece;

   return true;
}

bool PieceControl::SwapPieces(int nX1, int nY1, int nX2, int nY2)
{
   PieceSprite* pCurrent1 = NULL;
   for(pCurrent1 = m_pRoot; pCurrent1 != NULL; pCurrent1 = pCurrent1->next) {
      if( pCurrent1->boardX == nX1 && pCurrent1->boardY == nY1 )
        break;
   }

   PieceSprite* pCurrent2 = NULL;
   for(pCurrent2 = m_pRoot; pCurrent2 != NULL; pCurrent2 = pCurrent2->next) {
      if( pCurrent2->boardX == nX2 && pCurrent2->boardY == nY2 )
        break;
   }

   //Swap
   if( pCurrent1 ) {
      pCurrent1->boardX = nX2;
      pCurrent1->boardY = nY2;
      pCurrent1->toX = m_pBoardMetrics->GetXPos(nX2);
      pCurrent1->toY = m_pBoardMetrics->GetYPos(nY2);
   }

   if( pCurrent2 ) {
      pCurrent2->boardX = nX1;
      pCurrent2->boardY = nY1;
      pCurrent2->toX = m_pBoardMetrics->GetXPos(nX1);
      pCurrent2->toY = m_pBoardMetrics->GetYPos(nY1);
   }

   return true;
}

bool PieceControl::DropPiece(int nX, int nY)
{
   PieceSprite* pCurrent = NULL;
   for(pCurrent = m_pRoot; pCurrent != NULL; pCurrent = pCurrent->next) {
      if( pCurrent->boardX == nX && pCurrent->boardY == nY )
        break;
   }

   if( pCurrent ) {
      pCurrent->boardX = nX-1;
      pCurrent->boardY = nY;
      pCurrent->toX = m_pBoardMetrics->GetXPos(nX-1);
      pCurrent->toY = m_pBoardMetrics->GetYPos(nY);
   }

   return true;
}

bool PieceControl::DisappearPiece(int nX, int nY)
{
   PieceSprite* pCurrent = NULL;
   for(pCurrent = m_pRoot; pCurrent != NULL; pCurrent = pCurrent->next) {
      if( pCurrent->boardX == nX && pCurrent->boardY == nY )
        break;
   }

   if( pCurrent ) {
      pCurrent->toFade = true;
   }

   return true;
}

bool IsPieceMoving(PieceSprite* pSprite)
{
   return (pSprite->y != pSprite->toY) || (pSprite->x != pSprite->toX);
}

bool IsPieceFading(PieceSprite* pSprite, int nFadeLimit)
{
   return (pSprite->toFade == true && pSprite->nFadeAmount < nFadeLimit);
}

bool PieceControl::IsAnimating() const
{
   PieceSprite* pCurrent = NULL;
   for(pCurrent = m_pRoot; pCurrent != NULL; pCurrent = pCurrent->next) {
      if( IsPieceMoving(pCurrent) )
         return true;
      if( IsPieceFading(pCurrent, m_pConfig->GetFadeSteps()) )
         return true;
   }

   return false;
}

void SetBackground(SDL_Surface* pScreen, PieceSprite* pSprite, Metrics* pMetrics)
{
   SDL_Rect rect;
   rect.x = pSprite->x;
   rect.y = pSprite->y;
   rect.w = pMetrics->GetPieceSize();
   rect.h = pMetrics->GetPieceSize();

   SDL_BlitSurface(pScreen, &rect, pSprite->replace, NULL);
}

void MovePiece(SDL_Surface* pScreen, PieceSprite* pSprite, Metrics* pMetrics, int nMoveStepAmount)
{
   if( (pSprite->x != pSprite->toX) || (pSprite->y != pSprite->toY) ) {
      //int nDistX = pSprite->toX - pSprite->x;
      //int nDistY = pSprite->toY - pSprite->y;

      for(int n=0; n<nMoveStepAmount; n++) {
         if( pSprite->x == pSprite->toX )
            break;
         if( pSprite->x < pSprite->toX ) {
            pSprite->x++;
         }
         if( pSprite->x > pSprite->toX ) {
            pSprite->x--;
         }
      }

      for(int n=0; n<nMoveStepAmount; n++) {
         if( pSprite->y == pSprite->toY )
            break;
         if( pSprite->y < pSprite->toY ) {
            pSprite->y++;
         }
         if( pSprite->y > pSprite->toY ) {
            pSprite->y--;
         }
      }
   }

   SetBackground(pScreen, pSprite, pMetrics);
}

void Brighten(SDL_Surface* pSurface, int nBrightenAmount)
{
   //If the surface must be locked 
   if( SDL_MUSTLOCK( pSurface ) )
   {
      //Lock the surface 
      SDL_LockSurface( pSurface );
   }

   //Go through columns 
   for( int x = 0; x < pSurface->w; x++ )
   {
      //Go through rows 
      for( int y = 0; y < pSurface->h; y++ )
      {
         Uint32 pixel = nSDL_GetPixel(pSurface, x, y);
         Uint8 r = 0, g = 0, b = 0;
         SDL_GetRGB(pixel, pSurface->format, &r, &g, &b);
         r = Puz_Min(255, r+nBrightenAmount);
         g = Puz_Min(255, g+nBrightenAmount);
         b = Puz_Min(255, b+nBrightenAmount);

         nSDL_SetPixel(pSurface, x, y, SDL_MapRGB(pSurface->format, r, g, b));
      }
   }

   //Unlock surface 
   if( SDL_MUSTLOCK( pSurface ) )
   {
      SDL_UnlockSurface( pSurface );
   }
}

void FadePiece(SDL_Surface* pScreen, PieceSprite* pSprite, int nFadeLimit, int nBrightenAmount)
{
   if( pSprite->toFade == false )
      return;

   if( pSprite->nFadeAmount < nFadeLimit )
      pSprite->nFadeAmount++;

   Brighten(pSprite->img, nBrightenAmount);
}

void ShowPiece(SDL_Surface* pScreen, PieceSprite* pSprite, Metrics* pMetrics)
{
   SDL_Rect rect;
   rect.x = pSprite->x;
   rect.y = pSprite->y;
   rect.w = pMetrics->GetPieceSize();
   rect.h = pMetrics->GetPieceSize();

   SDL_BlitSurface(pSprite->img, NULL, pScreen, &rect);
}

void ResetBackground(SDL_Surface* pScreen, PieceSprite* pSprite, Metrics* pMetrics)
{
   SDL_Rect rect;
   rect.x = pSprite->x;
   rect.y = pSprite->y;
   rect.w = pMetrics->GetPieceSize();
   rect.h = pMetrics->GetPieceSize();

   SDL_BlitSurface(pSprite->replace, NULL, pScreen, &rect);
}

bool PieceControl::Animate()
{
   //if( !IsAnimating() ) {
      //return false;
   //}

   PieceSprite* pCurrent = m_pRoot;

   while(pCurrent != NULL) {
      MovePiece(m_pScreen, pCurrent, m_pBoardMetrics, m_pConfig->GetPieceMovePerStep());
      FadePiece(m_pScreen, pCurrent, m_pConfig->GetFadeSteps(), m_pConfig->GetBrightenPerStep());
      pCurrent = pCurrent->next;
   }

   //Check for any dead
   PieceSprite* pPrevious = NULL;
   pCurrent = m_pRoot;
   while(pCurrent != NULL) {
      PieceSprite* pTemp = pCurrent;
      pCurrent = pCurrent->next;
      if( pTemp->toFade == true && pTemp->nFadeAmount >= m_pConfig->GetFadeSteps() ) {
         SDL_FreeSurface(pTemp->img);
         SDL_FreeSurface(pTemp->replace);
         if( pPrevious != NULL ) {
            pPrevious->next = pTemp->next;
         }
         else {
            m_pRoot = pTemp->next;
         }
         free(pTemp);
      }
      else
      {
         pPrevious = pTemp;
      }
   }

   pCurrent = m_pRoot;
   while(pCurrent != NULL) {
      ShowPiece(m_pScreen, pCurrent, m_pBoardMetrics);
      pCurrent = pCurrent->next;
   }

   m_pSelector->DrawSelector();
   m_pMovesLabel->DrawLabel();
   SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

   pCurrent = m_pRoot;
   while(pCurrent != NULL) {
      ResetBackground(m_pScreen, pCurrent, m_pBoardMetrics);
      pCurrent = pCurrent->next;
   }

   return true;
}



