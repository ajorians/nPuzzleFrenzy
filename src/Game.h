#ifndef GAME_H
#define GAME_H

//#define USE_GRAPHIC_YOU_WIN//Use a static "You Win!!!" image instead of the animated one

extern "C"
{
	#include <os.h>
	#include "SDL/SDL.h"
	#include "PuzzleFrenzyLib/PuzzleFrenzyLib.h"
}

#ifndef USE_GRAPHIC_YOU_WIN
#include "Message.h"
#endif

#include "Config.h"
#include "Selector.h"
#include "MovesRemainingLabel.h"
#include "Metrics.h"
#include "Piece.h"
#include "Timer.h"
#include "Defines.h"

class Game
{
public:
	Game(SDL_Surface* pScreen, const char* pstrFile, int nLevelNumber, Config* pConfig);
	~Game();

	bool Loop();
	
protected:
	bool CheckStateConditions();
	bool PollEvents();
	void DrawSelector();
	void UpdateDisplay();
	void Move(Direction eDirection);
	void SwapCurrentCells();
	void Undo();
	void Redo();

protected:
	SDL_Surface	*m_pScreen;//Does not own
	SDL_Surface *m_pBackground;
	Message		m_StartMessage;
#ifdef USE_GRAPHIC_YOU_WIN
	SDL_Surface 	*m_pWinGraphic;
#else
	Message    m_YouWinMessage;
#endif
	nSDL_Font 	*m_pFont;
	PuzzleFrenzyLib	m_PuzzleFrenzy;
	int		m_nLevelNumber;
	Config		*m_pConfig;//Does not own
	Selector	m_Selector;
	MovesRemainingLabel	m_MovesRemainingLabel;
	bool		m_bGameOver;
	Metrics		m_BoardMetrics;
	PieceControl	m_Pieces;
	Timer		m_Timer;
};

#endif
