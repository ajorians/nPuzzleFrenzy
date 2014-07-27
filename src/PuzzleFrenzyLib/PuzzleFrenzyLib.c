//Public domain :)

#include <os.h>
#include "include/PuzzleFrenzyLib.h"
#include "Defines.h"

struct PuzzleFrenzyItem
{
   int m_nValue;
};

struct PuzzleFrenzyBoard
{
   int m_nWidth;
   int m_nHeight;
   struct PuzzleFrenzyItem* m_pItems;
};

struct PuzzleFrenzyItem* GetAt(struct PuzzleFrenzyBoard* pBoard, int nX, int nY)
{
   if( nX < 0 || nY < 0 || (nX >= pBoard->m_nWidth) || (nY >= pBoard->m_nHeight) ) {
      printf("Accessing non-existant element %d,%d\n", nX, nY);
      return NULL;
   }

   return pBoard->m_pItems + (pBoard->m_nWidth * nY + nX);
}

void* AllocateBoard(int nWidth, int nHeight)
{
   return malloc(nWidth*nHeight*sizeof(struct PuzzleFrenzyItem));
}

struct PuzzleFrenzyAction
{
   int m_nX;
   int m_nY;
   struct PuzzleFrenzyAction* m_pNext;
};

struct PuzzleFrenzy
{
   struct PuzzleFrenzyBoard* m_pBoard;
   char* m_pstrFile;
   int m_nNumberOfMoves;
   struct PuzzleFrenzyAction* m_pUndoActions;//Also use for move count :)
   struct PuzzleFrenzyAction* m_pRedoActions;
   int m_nLastError;
};

int PuzzleFrenzyLoadBoard(PuzzleFrenzyLib api, char* pstrFile)
{
   DEBUG_FUNC_NAME;

   struct PuzzleFrenzy* pH = (struct PuzzleFrenzy*)api;

   if( pH->m_pBoard != NULL ) {
      free(pH->m_pBoard);
      pH->m_pBoard = NULL;
   }

   pH->m_pBoard = malloc(sizeof(struct PuzzleFrenzyBoard));
   if( pH->m_pBoard == NULL ){//Out of memory
      free(pH);
      pH = NULL;
      return PUZZLEFRENZYLIB_OUT_OF_MEMORY;
   }
   pH->m_pBoard->m_pItems = NULL;

   if( strstr(pstrFile, "PuzzleFrenzy ") != pstrFile ) {//PuzzleFrenzy file version check
      free(pH->m_pBoard);
      pH->m_pBoard = NULL;
      free(pH);
      pH = NULL;
      return PUZZLEFRENZYLIB_NOT_PUZZLE_LEVEL;
   }

   if( strstr(pstrFile, "1 ") != (pstrFile + strlen("PuzzleFrenzy ")) ) {//Version check
      free(pH->m_pBoard);
      pH->m_pBoard = NULL;
      free(pH);
      pH = NULL;
      return PUZZLEFRENZYLIB_UNKNOWN_VERSION;
   }

   char* pstr = pstrFile + strlen("PuzzleFrenzy 1 ");
   char buffer[16];
   int nSpotInBuffer = 0;

   int nNumberOfMoves = -1, nWidth = -1, nHeight = -1;
   int nX = -1, nY = -1;
   while(pstr != '\0') {
      char ch = *pstr; pstr++;
      if( isdigit(ch) ) {
         buffer[nSpotInBuffer++] = ch;
      }
      else {
         if( !isspace(ch) )
            break;
         buffer[nSpotInBuffer] = '\0';
         nSpotInBuffer = 0;
         int nValue = atoi(buffer);

         if( nNumberOfMoves < 0 ) {
            nNumberOfMoves = nValue;
            pH->m_nNumberOfMoves = nNumberOfMoves;
         }
         else if( nWidth < 0 ) {
            nWidth = nValue;
            pH->m_pBoard->m_nWidth = nWidth;
         }
         else if( nHeight < 0 ) {
            nHeight = nValue;
            pH->m_pBoard->m_nHeight = nHeight;
            pH->m_pBoard->m_pItems = malloc(nWidth*nHeight*sizeof(struct PuzzleFrenzyItem));

            if( pH->m_pBoard->m_pItems == NULL ) {//Out of memory
               free(pH->m_pBoard);
               pH->m_pBoard = NULL;
               free(pH);
               pH = NULL;
               return PUZZLEFRENZYLIB_OUT_OF_MEMORY;
            }

            int x,y;
            for(x=0; x<nWidth; x++)
               for(y=0; y<nHeight; y++)
                  GetAt(pH->m_pBoard, x, y)->m_nValue = 0;

         }
         else {
            if( nX <= -1 ) {
               nX = nValue;
            }
            else if( nY <= -1 ) {
               nY = nValue;
            }
            else {
               struct PuzzleFrenzyItem* pItem = GetAt(pH->m_pBoard, nX, nY);
               pItem->m_nValue = nValue;
               nX = nY = -1;
            }

            //Maybe check if has more numbers than it should?
         }
      }
   }
   if( nX >= 0 && nY >= 0 ) {
      buffer[nSpotInBuffer] = '\0';
      int nValue = atoi(buffer);
      struct PuzzleFrenzyItem* pItem = GetAt(pH->m_pBoard, nX, nY);
      pItem->m_nValue = nValue;
   }

   return PUZZLEFRENZYLIB_OK;
}

int PuzzleFrenzyLibCreate(PuzzleFrenzyLib* api, const char* pstrFile)
{
   DEBUG_FUNC_NAME;

   struct PuzzleFrenzy* pH = malloc(sizeof(struct PuzzleFrenzy));
   if( pH == NULL ){//Out of memory
      return PUZZLEFRENZYLIB_OUT_OF_MEMORY;
   }

   pH->m_pstrFile = pstrFile;
   pH->m_pBoard = NULL;

   int nRet = PuzzleFrenzyLoadBoard((PuzzleFrenzyLib)pH, pstrFile);
   if( nRet != PUZZLEFRENZYLIB_OK )
      return nRet;

   pH->m_pUndoActions = NULL;
   pH->m_pRedoActions = NULL;
   pH->m_nLastError = PUZZLEFRENZYLIB_OK;

   *api = pH;

   return PUZZLEFRENZYLIB_OK;
}

void ClearUndos(PuzzleFrenzyLib api)
{
   DEBUG_FUNC_NAME;

   struct PuzzleFrenzy* pH = (struct PuzzleFrenzy*)api;

   struct PuzzleFrenzyAction* pCurrent = pH->m_pUndoActions;
   while(pCurrent != NULL) {
      struct PuzzleFrenzyAction* pTemp = pCurrent;
      pCurrent = pCurrent->m_pNext;
      free(pTemp);
      pTemp = NULL;
      pH->m_pUndoActions = pCurrent;
   }

}

void ClearRedos(PuzzleFrenzyLib api)
{
   DEBUG_FUNC_NAME;

   struct PuzzleFrenzy* pH = (struct PuzzleFrenzy*)api;

   struct PuzzleFrenzyAction* pCurrent = pH->m_pRedoActions;
   while(pCurrent != NULL) {
      struct PuzzleFrenzyAction* pTemp = pCurrent;
      pCurrent = pCurrent->m_pNext;
      free(pTemp);
      pTemp = NULL;
      pH->m_pRedoActions = pCurrent;
   }

}

void AddUndo(PuzzleFrenzyLib api, int nX, int nY)
{
   DEBUG_FUNC_NAME;

   struct PuzzleFrenzy* pH = (struct PuzzleFrenzy*)api;

   struct PuzzleFrenzyAction* pAction = malloc(sizeof(struct PuzzleFrenzyAction));
   if( pAction == NULL ) {
      DEBUG_MSG("Out of memory: AddUndo\n");
   //Assume allocated
   }

   pAction->m_nX = nX;
   pAction->m_nY = nY;

   struct PuzzleFrenzyAction* pRoot = pH->m_pUndoActions;
   pAction->m_pNext = pRoot;
   pH->m_pUndoActions = pAction;
}

void AddRedo(PuzzleFrenzyLib api, int nX, int nY)
{
   DEBUG_FUNC_NAME;

   struct PuzzleFrenzy* pH = (struct PuzzleFrenzy*)api;

   struct PuzzleFrenzyAction* pAction = malloc(sizeof(struct PuzzleFrenzyAction));
   if( pAction == NULL ) {
      DEBUG_MSG("Out of memory: AddRedo\n");
      //Assume allocated
   }

   pAction->m_nX = nX;
   pAction->m_nY = nY;

   struct PuzzleFrenzyAction* pRoot = pH->m_pRedoActions;
   pAction->m_pNext = pRoot;
   pH->m_pRedoActions = pAction;
}

int PuzzleFrenzyLibFree(PuzzleFrenzyLib* api)
{
   DEBUG_FUNC_NAME;

   struct PuzzleFrenzy* pH = *api;

   ClearUndos(*api);
   ClearRedos(*api);

   free(pH->m_pBoard->m_pItems);
   pH->m_pBoard->m_pItems = NULL;

   free(pH->m_pBoard);
   pH->m_pBoard = NULL;
   free(pH);
   pH = NULL;

   *api = NULL;
   return PUZZLEFRENZYLIB_OK;
}

int GetPuzzleFrenzyError(PuzzleFrenzyLib api)
{
   DEBUG_FUNC_NAME;

   struct PuzzleFrenzy* pH = (struct PuzzleFrenzy*)api;
   return pH->m_nLastError;
}

void ClearPuzzleFrenzyError(PuzzleFrenzyLib api)
{
   DEBUG_FUNC_NAME;

   struct PuzzleFrenzy* pH = (struct PuzzleFrenzy*)api;
   pH->m_nLastError = PUZZLEFRENZYLIB_OK;
}

//PuzzleFrenzyLib related functions
int GetPuzzleFrenzyBoardWidth(PuzzleFrenzyLib api)
{
   DEBUG_FUNC_NAME;

   struct PuzzleFrenzy* pH = (struct PuzzleFrenzy*)api;

   return pH->m_pBoard->m_nWidth;
}

int GetPuzzleFrenzyBoardHeight(PuzzleFrenzyLib api)
{
   DEBUG_FUNC_NAME;

   struct PuzzleFrenzy* pH = (struct PuzzleFrenzy*)api;

   return pH->m_pBoard->m_nHeight;
}

int GetPuzzleFrenzySpotValue(PuzzleFrenzyLib api, int nX, int nY)
{
   DEBUG_FUNC_NAME;

   struct PuzzleFrenzy* pH = (struct PuzzleFrenzy*)api;

   return GetAt(pH->m_pBoard, nX, nY)->m_nValue;
}

int IsPuzzleFrenzySolved(PuzzleFrenzyLib api)
{
   DEBUG_FUNC_NAME;

   struct PuzzleFrenzy* pH = (struct PuzzleFrenzy*)api;

   int nX = 0, nY = 0;
   for(nX = 0; nX<GetPuzzleFrenzyBoardWidth(api); nX++)
      for(nY =0; nY<GetPuzzleFrenzyBoardHeight(api); nY++)
         if( GetPuzzleFrenzySpotValue(api, nX, nY) > 0 )
            return PUZZLEFRENZYLIB_NOTSOLVED;

   return PUZZLEFRENZYLIB_SOLVED;
}

int GetPuzzleFrenzyMoveLimit(PuzzleFrenzyLib api)
{
   DEBUG_FUNC_NAME;

   struct PuzzleFrenzy* pH = (struct PuzzleFrenzy*)api;

   return pH->m_nNumberOfMoves;
}

int GetPuzzleFrenzyMovesMadeSoFar(PuzzleFrenzyLib api)
{
   DEBUG_FUNC_NAME;

   struct PuzzleFrenzy* pH = (struct PuzzleFrenzy*)api;

   int nCount = 0;
   struct PuzzleFrenzyAction* pCurrent = pH->m_pUndoActions;
   while(pCurrent != NULL) {
      nCount++;
      pCurrent = pCurrent->m_pNext;
   }

   return nCount;
}

int ShouldPuzzleFrenzyPieceDrop(PuzzleFrenzyLib api, int nX, int nY)
{
   DEBUG_FUNC_NAME;

   struct PuzzleFrenzy* pH = (struct PuzzleFrenzy*)api;

   if( nX < 1 ) {//Can't drop
      return PUZZLEFRENZYLIB_CANNOT_DROP;
   }

   if( GetPuzzleFrenzySpotValue(api, nX, nY) == 0 ) {
      return PUZZLEFRENZYLIB_NO_PIECE_NO_DROP;
   }

   if( GetPuzzleFrenzySpotValue(api, nX-1, nY) == 0 ) {
      return PUZZLEFRENZYLIB_SHOULD_DROP;
   }

   return PUZZLEFRENZYLIB_NO_DROP;
}

int DropPuzzleFrenzyPiece(PuzzleFrenzyLib api, int nX, int nY)
{
   DEBUG_FUNC_NAME;

   struct PuzzleFrenzy* pH = (struct PuzzleFrenzy*)api;

   if( PUZZLEFRENZYLIB_SHOULD_DROP == ShouldPuzzleFrenzyPieceDrop(api, nX, nY) ) {
     GetAt(pH->m_pBoard, nX-1, nY)->m_nValue = GetAt(pH->m_pBoard, nX, nY)->m_nValue;
     GetAt(pH->m_pBoard, nX, nY)->m_nValue = 0;
     return PUZZLEFRENZYLIB_DROPPED_PIECE;
    }

   return PUZZLEFRENZYLIB_DIDNOT_DROP_PIECE;
}

int DoPuzzleFrenzyDropping(PuzzleFrenzyLib api)
{
   DEBUG_FUNC_NAME;

   struct PuzzleFrenzy* pH = (struct PuzzleFrenzy*)api;

   int nAnyChanges = 0;
   int nX = 0, nY = 0;
   for(nY =0; nY<GetPuzzleFrenzyBoardHeight(api); nY++) {
      for(nX = 1; nX<GetPuzzleFrenzyBoardWidth(api); nX++) {
         if( PUZZLEFRENZYLIB_DROPPED_PIECE == DropPuzzleFrenzyPiece(api, nX, nY) ) {
            nAnyChanges = 1;
         }
      }
   }
   return nAnyChanges;
}

int ShouldPuzzleFrenzyPieceDisappear(PuzzleFrenzyLib api, int nX, int nY)
{
   DEBUG_FUNC_NAME;

   struct PuzzleFrenzy* pH = (struct PuzzleFrenzy*)api;

   int nType = GetPuzzleFrenzySpotValue(api, nX, nY);

   if( nType <= 0 )
      return PUZZLEFRENZYLIB_NO_DIAPPEAR;

   //Testing with all four sides
   //Test Left
   if( nX >= 2 && GetPuzzleFrenzySpotValue(api, nX-2, nY) == nType && GetPuzzleFrenzySpotValue(api, nX-1, nY) == nType ) {
      return PUZZLEFRENZYLIB_SHOULD_DISAPPEAR_LEFT;
   }
   //Test Top
   if( nY >= 2 && GetPuzzleFrenzySpotValue(api, nX, nY-2) == nType && GetPuzzleFrenzySpotValue(api, nX, nY-1) == nType ) {
      return PUZZLEFRENZYLIB_SHOULD_DISAPPEAR_TOP;
   }
   //Test Right
   if( nX < GetPuzzleFrenzyBoardWidth(api)-2 && GetPuzzleFrenzySpotValue(api, nX+2, nY) == nType && GetPuzzleFrenzySpotValue(api, nX+1, nY) == nType ) {
      return PUZZLEFRENZYLIB_SHOULD_DISAPPEAR_RIGHT;
   }
   //Test Down
   if( nY < GetPuzzleFrenzyBoardHeight(api)-2 && GetPuzzleFrenzySpotValue(api, nX, nY+2) == nType && GetPuzzleFrenzySpotValue(api, nX, nY+1) == nType ) {
      return PUZZLEFRENZYLIB_SHOULD_DISAPPEAR_DOWN;
   }

   //Though I could add all 3 spots; I test a spot at a time.  So here I am testing the sides
   if( nY >= 1 && nY < GetPuzzleFrenzyBoardHeight(api)-1 && GetPuzzleFrenzySpotValue(api, nX, nY-1) == nType && GetPuzzleFrenzySpotValue(api, nX, nY+1) == nType ) {
      return PUZZLEFRENZYLIB_SHOULD_DISAPPEAR_MIDV;
   }
   if( nX >= 1 && nX < GetPuzzleFrenzyBoardWidth(api)-1 && GetPuzzleFrenzySpotValue(api, nX-1, nY) == nType && GetPuzzleFrenzySpotValue(api, nX+1, nY) == nType ) {
      return PUZZLEFRENZYLIB_SHOULD_DISAPPEAR_MIDH;
   }

   return PUZZLEFRENZYLIB_NO_DIAPPEAR;
}

struct PuzzleFrenzyMatches
{
   int m_nX;
   int m_nY;
   struct PuzzleFrenzyMatches* m_pNext;
};

int PuzzleFrenzyRemoveMatches(PuzzleFrenzyLib api)
{
   DEBUG_FUNC_NAME;

   struct PuzzleFrenzy* pH = (struct PuzzleFrenzy*)api;

   struct PuzzleFrenzyMatches* pRoot = NULL;

   int nAnyChanges = 0;
   int nX = 0, nY = 0;
   for(nX = 0; nX<GetPuzzleFrenzyBoardWidth(api); nX++) {
      for(nY =0; nY<GetPuzzleFrenzyBoardHeight(api); nY++) {

         if( PUZZLEFRENZYLIB_SHOULD_DISAPPEAR == ShouldPuzzleFrenzyPieceDisappear(api, nX, nY) ) {
            nAnyChanges = 1;
            struct PuzzleFrenzyMatches* pNode = malloc(sizeof(struct PuzzleFrenzyMatches));
            //Assume always succeeds
            pNode->m_pNext = pRoot;
            pNode->m_nX = nX;
            pNode->m_nY = nY;
            pRoot = pNode;
         }
      }
   }

   while(pRoot != NULL) {
      struct PuzzleFrenzyMatches* pNode = pRoot;
      pRoot = pNode->m_pNext;
      GetAt(pH->m_pBoard, pNode->m_nX, pNode->m_nY)->m_nValue = 0;
      free(pNode);
   }
   return nAnyChanges;
}

int SwapPuzzleFrenzySpots(PuzzleFrenzyLib api, int nX, int nY, int nIsUndo)
{
   DEBUG_FUNC_NAME;

   struct PuzzleFrenzy* pH = (struct PuzzleFrenzy*)api;
   if( GetPuzzleFrenzyMovesMadeSoFar(api) >= pH->m_nNumberOfMoves )
      return PUZZLEFRENZYLIB_CANNOT_SWAP_MOVE_LIMIT;

   if( nY < 0 || (nY+2)>GetPuzzleFrenzyBoardHeight(api) || nX < 0 || nX >= GetPuzzleFrenzyBoardWidth(api))
      return PUZZLEFRENZYLIB_SWAP_OFF_BOARD;

   if( GetPuzzleFrenzySpotValue(api, nX, nY) == GetPuzzleFrenzySpotValue(api, nX, nY+1) )
      return PUZZLEFRENZYLIB_CANNOT_SWAP_SAME_TYPE;

   if( !nIsUndo ) {
      ClearRedos(api);

      AddUndo(api, nX, nY);
   }

   int nTemp = GetAt(pH->m_pBoard, nX, nY)->m_nValue;
   GetAt(pH->m_pBoard, nX, nY)->m_nValue = GetAt(pH->m_pBoard, nX, nY+1)->m_nValue;
   GetAt(pH->m_pBoard, nX, nY+1)->m_nValue = nTemp;

   return PUZZLEFRENZYLIB_CAN_SWAP;
}

void DoUndo(PuzzleFrenzyLib api, struct PuzzleFrenzyAction* pUndo)
{
   if( pUndo == NULL )
      return;

   DoUndo(api, pUndo->m_pNext);

   SwapPuzzleFrenzySpots(api, pUndo->m_nX, pUndo->m_nY, 1);
   while(1) {
      if( DoPuzzleFrenzyDropping(api) )
         continue;

      if( PuzzleFrenzyRemoveMatches(api) )
         continue;
      
      break;
   }
}

int PuzzleFrenzyUndo(PuzzleFrenzyLib api)
{
   DEBUG_FUNC_NAME;

   struct PuzzleFrenzy* pH = (struct PuzzleFrenzy*)api;

   struct PuzzleFrenzyAction* pRoot = pH->m_pUndoActions;
   if( pRoot == NULL )
      return PUZZLEFRENZYLIB_CANNOT_UNDO;

   pH->m_pUndoActions = pRoot->m_pNext;
   AddRedo(api, pRoot->m_nX, pRoot->m_nY);

   PuzzleFrenzyLoadBoard(api, pH->m_pstrFile);   

   //Now loop over the undos; except the first one
   DoUndo(api, pH->m_pUndoActions);

   free(pRoot);
   pRoot = NULL;

   return PUZZLEFRENZYLIB_OK;
}

int PuzzleFrenzyRedo(PuzzleFrenzyLib api)
{
   DEBUG_FUNC_NAME;

   struct PuzzleFrenzy* pH = (struct PuzzleFrenzy*)api;

   struct PuzzleFrenzyAction* pRoot = pH->m_pRedoActions;
   if( pRoot == NULL )
      return PUZZLEFRENZYLIB_CANNOT_REDO;

   pH->m_pRedoActions = pRoot->m_pNext;
   AddUndo(api, pRoot->m_nX, pRoot->m_nY);

   SwapPuzzleFrenzySpots(api, pRoot->m_nX, pRoot->m_nY, 1);

   free(pRoot);
   pRoot = NULL;

   return PUZZLEFRENZYLIB_OK;
}


