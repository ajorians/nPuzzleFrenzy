#include "Game.h"
#include "GameBackground.h"
#ifdef USE_GRAPHIC_YOU_WIN
#include "YouWinGraphic.h"
#endif

extern "C"
{
#include "SDL/SDL_gfxPrimitives.h"
}

Game::Game(SDL_Surface* pScreen, const char* pstrFile, int nLevelNumber, Config* pConfig)
: m_pScreen(pScreen), m_StartMessage(pScreen)
#ifdef USE_GRAPHIC_YOU_WIN
, m_pWinGraphic(NULL)
#else
, m_YouWinMessage(pScreen)
#endif
, m_nLevelNumber(nLevelNumber), m_pConfig(pConfig), m_Selector(pScreen, pConfig, &m_BoardMetrics), m_MovesRemainingLabel(pScreen, &m_BoardMetrics), m_bGameOver(false), m_Pieces(pScreen, &m_BoardMetrics, pConfig, &m_Selector, &m_MovesRemainingLabel)
{
	PuzzleFrenzyLibCreate(&m_PuzzleFrenzy, pstrFile);
	m_MovesRemainingLabel.SetPuzzleFrenzyLib(&m_PuzzleFrenzy);

	int nWidth = GetPuzzleFrenzyBoardWidth(m_PuzzleFrenzy), nHeight = GetPuzzleFrenzyBoardHeight(m_PuzzleFrenzy);

	m_BoardMetrics.SetDimensions(nWidth, nHeight);

	m_pBackground = nSDL_LoadImage(image_PuzzleFrenzyGameBackground);
	m_pFont = nSDL_LoadFont(NSDL_FONT_THIN, 0/*R*/, 0/*G*/, 0/*B*/);

        for(int nX=0; nX<nWidth; nX++) {
                for(int nY=0; nY<nHeight; nY++) {
			int nType = GetPuzzleFrenzySpotValue(m_PuzzleFrenzy, nX, nY);
			if( nType <= 0 ) continue;

			m_Pieces.CreatePiece(nType, nX, nY);
                }
        }

	if( m_pConfig->GetShowOpeningMoveLimit() ) {
		char buffer[32];
		char buf[8];
		strcpy(buffer, "Move\nLimit: ");
		Puz_itoa(GetPuzzleFrenzyMoveLimit(m_PuzzleFrenzy), buf, 8);
		strcat(buffer, buf);
		m_StartMessage.CreateMessage(buffer);
	}
}

Game::~Game()
{
	PuzzleFrenzyLibFree(&m_PuzzleFrenzy);
	SDL_FreeSurface(m_pBackground);
#ifdef USE_GRAPHIC_YOU_WIN
	if( m_pWinGraphic != NULL )
		SDL_FreeSurface(m_pWinGraphic);
#endif
	nSDL_FreeFont(m_pFont);
}

bool Game::Loop()
{
	//Handle keypresses
	if( PollEvents() == false )
		return false;

	//Check for state changes
	if( CheckStateConditions() == false )
		return false;

	//Update screen
	UpdateDisplay();
	
	SDL_Delay(30);
	
	return true;
}

bool Game::CheckStateConditions()
{
   if( m_StartMessage.HasMessage() && ((!m_StartMessage.IsAnimating() && !m_StartMessage.IsStayDuration()) || m_StartMessage.FinishFast()) && !m_bGameOver ) {
      m_StartMessage.ClearMessage();
      m_Timer.Start();
   }

   if( !m_Pieces.IsAnimating() ) {
      bool bAnyDropping = false;
      for(int nX=0; nX<GetPuzzleFrenzyBoardWidth(m_PuzzleFrenzy); nX++) {
         for(int nY=0; nY<GetPuzzleFrenzyBoardHeight(m_PuzzleFrenzy); nY++) {
            if( PUZZLEFRENZYLIB_SHOULD_DROP == ShouldPuzzleFrenzyPieceDrop(m_PuzzleFrenzy, nX, nY ) ) {
               m_Pieces.DropPiece(nX, nY);
               DropPuzzleFrenzyPiece(m_PuzzleFrenzy, nX, nY);
               bAnyDropping = true;
            }
         }
      }

      if( bAnyDropping )
         return true;

      bool bAnyFading = false;
      for(int nX=0; nX<GetPuzzleFrenzyBoardWidth(m_PuzzleFrenzy); nX++) {
         for(int nY=0; nY<GetPuzzleFrenzyBoardHeight(m_PuzzleFrenzy); nY++) {
            if( PUZZLEFRENZYLIB_SHOULD_DISAPPEAR == ShouldPuzzleFrenzyPieceDisappear(m_PuzzleFrenzy, nX, nY ) ) {
               m_Pieces.DisappearPiece(nX, nY);
               bAnyFading = true;
            }
         }
      }
      PuzzleFrenzyRemoveMatches(m_PuzzleFrenzy);

      if( bAnyFading )
         return true;

      if( !m_bGameOver && IsPuzzleFrenzySolved(m_PuzzleFrenzy) == PUZZLEFRENZYLIB_SOLVED ) {
         m_bGameOver = true;
#ifndef USE_GRAPHIC_YOU_WIN
         m_YouWinMessage.CreateMessage("Level\nComplete!!\n******");
#endif
         m_Timer.Stop();
         if( m_nLevelNumber >= 0 ) {
            m_pConfig->SetBeatLevel(m_nLevelNumber, m_Timer.GetElapsed());
         }
      }

      if( m_YouWinMessage.HasMessage() && ((!m_YouWinMessage.IsAnimating() && !m_YouWinMessage.IsStayDuration()) || m_YouWinMessage.FinishFast()) )
         return false;
   }

   return true;
}

bool Game::PollEvents()
{
	SDL_Event event;
	
	/* Poll for events. SDL_PollEvent() returns 0 when there are no  */
	/* more events on the event queue, our while loop will exit when */
	/* that occurs.                                                  */
	while( SDL_PollEvent( &event ) )
	{
		/* We are only worried about SDL_KEYDOWN and SDL_KEYUP events */
		switch( event.type )
		{
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) 
				{
					case SDLK_ESCAPE:
						fprintf(stderr, "Hit Escape!n");
					return false;
					break;

					case SDLK_RETURN:
                                        case SDLK_LCTRL:
                                        case SDLK_RCTRL:
                                                SwapCurrentCells();
                                        break;
					
					case SDLK_UP:
                                        case SDLK_8:
                                                Move(Up);
        	                                break;
                                        
                                        case SDLK_DOWN:
                                        case SDLK_2:
                                                Move(Down);
	                                        break;

					case SDLK_RIGHT:
					case SDLK_6:
						Move(Right);
						break;
					
					case SDLK_LEFT:
					case SDLK_4:
						Move(Left);
						break;

					case SDLK_PLUS:
						Redo();
						break;

					case SDLK_MINUS:
						Undo();
						break;

					default:
						break;
				}
				break;
			
			//Called when the mouse moves
			case SDL_MOUSEMOTION:
				break;
			
			case SDL_KEYUP:
				break;
			
			default:
				break;
		}
	}
	return true;
}

void Game::DrawSelector()
{
   //m_Selector.DrawSelector();
}

void Game::UpdateDisplay()
{
	//Draw background
	if( !is_classic ) {
		SDL_BlitSurface(m_pBackground, NULL, m_pScreen, NULL);
	} else {
		SDL_FillRect(m_pScreen, NULL, SDL_MapRGB(m_pScreen->format, 255, 255, 255));
	}

	boxRGBA(m_pScreen, m_BoardMetrics.GetLeft(), m_BoardMetrics.GetTop(), m_BoardMetrics.GetRight(), m_BoardMetrics.GetBottom(),  GAME_BACKGROUND_R, GAME_BACKGROUND_G, GAME_BACKGROUND_B, 230);

	if( m_StartMessage.HasMessage() && m_StartMessage.Animate() ){ return; }
	if( !m_YouWinMessage.HasMessage() && !m_Pieces.Animate() )
	{
		DrawSelector();
		SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	}

#ifdef USE_GRAPHIC_YOU_WIN
	if( m_bGameOver ) {
		if( m_pWinGraphic == NULL ) {
		m_pWinGraphic = nSDL_LoadImage(image_HitoriYouWin);
                SDL_SetColorKey(m_pWinGraphic, SDL_SRCCOLORKEY, SDL_MapRGB(m_pWinGraphic->format, 255, 255, 255));
		}

		SDL_Rect rectWin;
		rectWin.x = (SCREEN_WIDTH - m_pWinGraphic->w)/2;
		rectWin.y = (SCREEN_HEIGHT - m_pWinGraphic->h)/2;
		rectWin.w = m_pWinGraphic->w;
		rectWin.h = m_pWinGraphic->h;
	
		SDL_BlitSurface(m_pWinGraphic, NULL, m_pScreen, &rectWin);	
	}
#else
	if( m_YouWinMessage.HasMessage() && !m_YouWinMessage.Animate() )
#endif
        {
                //SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        }
}

void Game::Move(Direction eDirection)
{
	if( m_bGameOver )
		return;

	m_Selector.Move(eDirection);	
}

void Game::SwapCurrentCells()
{
	if( m_StartMessage.HasMessage() && (m_StartMessage.IsAnimating() || m_StartMessage.IsStayDuration()) ) {
		m_StartMessage.SetFinishFast();
		return;
	}
	if( m_YouWinMessage.HasMessage() && (m_YouWinMessage.IsAnimating() || m_YouWinMessage.IsStayDuration()) ) {
		m_YouWinMessage.SetFinishFast();
		return;
	}

	if( m_bGameOver || m_Pieces.IsAnimating() )
                return;

	if( PUZZLEFRENZYLIB_CAN_SWAP == SwapPuzzleFrenzySpots(m_PuzzleFrenzy, m_Selector.GetCurrentX(), m_Selector.GetCurrentY(), 0 ) ) {
		m_Pieces.SwapPieces(m_Selector.GetCurrentX(), m_Selector.GetCurrentY(), m_Selector.GetCurrentX(), m_Selector.GetCurrentY()+1);
	}
}

void Game::Undo()
{
   if( m_bGameOver )
      return;

   if( PUZZLEFRENZYLIB_CANNOT_UNDO != PuzzleFrenzyUndo(m_PuzzleFrenzy) ) {
      m_Pieces.ClearPieces();

      int nWidth = GetPuzzleFrenzyBoardWidth(m_PuzzleFrenzy), nHeight = GetPuzzleFrenzyBoardHeight(m_PuzzleFrenzy);


      for(int nX=0; nX<nWidth; nX++) {
         for(int nY=0; nY<nHeight; nY++) {
            int nType = GetPuzzleFrenzySpotValue(m_PuzzleFrenzy, nX, nY);
            if( nType <= 0 ) continue;

             m_Pieces.CreatePiece(nType, nX, nY);
          }
      }      
   }

}

void Game::Redo()
{
   if( m_bGameOver )
      return;

   if( PUZZLEFRENZYLIB_CANNOT_REDO != PuzzleFrenzyRedo(m_PuzzleFrenzy) ) {
      m_Pieces.ClearPieces();

      int nWidth = GetPuzzleFrenzyBoardWidth(m_PuzzleFrenzy), nHeight = GetPuzzleFrenzyBoardHeight(m_PuzzleFrenzy);


      for(int nX=0; nX<nWidth; nX++) {
         for(int nY=0; nY<nHeight; nY++) {
            int nType = GetPuzzleFrenzySpotValue(m_PuzzleFrenzy, nX, nY);
            if( nType <= 0 ) continue;

             m_Pieces.CreatePiece(nType, nX, nY);
          }
      }
   }
}

