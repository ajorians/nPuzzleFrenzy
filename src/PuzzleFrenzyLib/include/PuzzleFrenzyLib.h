#ifndef PUZZLEFRENZYLIB_H_INCLUDED
#define PUZZLEFRENZYLIB_H_INCLUDED

typedef void* PuzzleFrenzyLib;

#define PUZZLEFRENZYLIB_OK			(0)
#define PUZZLEFRENZYLIB_BADARGUMENT		(-1)
#define PUZZLEFRENZYLIB_OUT_OF_MEMORY		(-2)
#define PUZZLEFRENZYLIB_NOT_PUZZLE_LEVEL	(-3)
#define PUZZLEFRENZYLIB_UNKNOWN_VERSION		(-4)

#define PUZZLEFRENZYLIB_SOLVED			(0)
#define PUZZLEFRENZYLIB_NOTSOLVED		(-1)

#define PUZZLEFRENZYLIB_CANNOT_UNDO		(1)
#define PUZZLEFRENZYLIB_CANNOT_REDO		(1)

#define PUZZLEFRENZYLIB_CAN_SWAP		(0)
#define PUZZLEFRENZYLIB_CANNOT_SWAP_MOVE_LIMIT  (1)
#define PUZZLEFRENZYLIB_CANNOT_SWAP_SAME_TYPE	(2)
#define PUZZLEFRENZYLIB_SWAP_OFF_BOARD		(3)

#define PUZZLEFRENZYLIB_SHOULD_DROP		(0)
#define PUZZLEFRENZYLIB_NO_DROP			(1)
#define PUZZLEFRENZYLIB_CANNOT_DROP		(2)
#define PUZZLEFRENZYLIB_NO_PIECE_NO_DROP	(3)

#define PUZZLEFRENZYLIB_DROPPED_PIECE		(0)
#define PUZZLEFRENZYLIB_DIDNOT_DROP_PIECE	(1)

#define PUZZLEFRENZYLIB_SHOULD_DISAPPEAR	(0)
#define PUZZLEFRENZYLIB_SHOULD_DISAPPEAR_LEFT	(PUZZLEFRENZYLIB_SHOULD_DISAPPEAR)
#define PUZZLEFRENZYLIB_SHOULD_DISAPPEAR_TOP	(PUZZLEFRENZYLIB_SHOULD_DISAPPEAR)
#define PUZZLEFRENZYLIB_SHOULD_DISAPPEAR_RIGHT	(PUZZLEFRENZYLIB_SHOULD_DISAPPEAR)
#define PUZZLEFRENZYLIB_SHOULD_DISAPPEAR_DOWN	(PUZZLEFRENZYLIB_SHOULD_DISAPPEAR)
#define PUZZLEFRENZYLIB_SHOULD_DISAPPEAR_MIDV	(PUZZLEFRENZYLIB_SHOULD_DISAPPEAR)
#define PUZZLEFRENZYLIB_SHOULD_DISAPPEAR_MIDH	(PUZZLEFRENZYLIB_SHOULD_DISAPPEAR)
#define PUZZLEFRENZYLIB_NO_DIAPPEAR		(1)

//////////////////////////////////////////////
//Initalization/Error checking/Mode functions
//////////////////////////////////////////////
int PuzzleFrenzyLibCreate(PuzzleFrenzyLib* api, const char* pstrFile );
int PuzzleFrenzyLibFree(PuzzleFrenzyLib* api);

int GetPuzzleFrenzyLibError(PuzzleFrenzyLib api);
void ClearPuzzleFrenzyLibError(PuzzleFrenzyLib api);

//////////////////////////////////////////////
//PuzzleFrenzyLib related functions
//////////////////////////////////////////////
int GetPuzzleFrenzyBoardWidth(PuzzleFrenzyLib api);
int GetPuzzleFrenzyBoardHeight(PuzzleFrenzyLib api);
int GetPuzzleFrenzySpotValue(PuzzleFrenzyLib api, int nX, int nY);
int IsPuzzleFrenzySolved(PuzzleFrenzyLib api);
int GetPuzzleFrenzyMoveLimit(PuzzleFrenzyLib api);
int GetPuzzleFrenzyMovesMadeSoFar(PuzzleFrenzyLib api);
int ShouldPuzzleFrenzyPieceDrop(PuzzleFrenzyLib api, int nX, int nY);
int DropPuzzleFrenzyPiece(PuzzleFrenzyLib api, int nX, int nY);
int DoPuzzleFrenzyDropping(PuzzleFrenzyLib api);
int ShouldPuzzleFrenzyPieceDisappear(PuzzleFrenzyLib api, int nX, int nY);
int PuzzleFrenzyRemoveMatches(PuzzleFrenzyLib api);
int SwapPuzzleFrenzySpots(PuzzleFrenzyLib api, int nX, int nY, int nIsUndo);
int PuzzleFrenzyUndo(PuzzleFrenzyLib api);
int PuzzleFrenzyRedo(PuzzleFrenzyLib api);

#endif //PUZZLEFRENZYLIB_H_INCLUDED
