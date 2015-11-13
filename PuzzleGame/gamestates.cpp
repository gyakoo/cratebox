#include <gamestates.h>
#include <stdarg.h>
#include <board.h>

namespace PuzzleGame
{

  GameStatePlaying::GameStatePlaying(std::shared_ptr<FSMManager> fsm, std::shared_ptr<Engine> engine)
    : m_fsm(fsm), m_engine(engine), m_timer(engine)
  {
  }

  void GameStatePlaying::OnEnter()
  {
    m_board = std::make_shared<Board>(m_engine, BOARD_DIM_DEFAULT, TILE_DIM_DEFAULT, TILE_DIM_DEFAULT);
    //m_timer.AddCallback( std::chrono::milliseconds(2500), std::bind(&GameStatePlaying::OnCreatePiece, this) );
  }

  void GameStatePlaying::OnUpdate()
  {
    m_timer.Update();

    m_board->Draw();
  }

  void GameStatePlaying::OnExit()
  {
    m_board = nullptr;
    m_timer.Reset();
  }

  void GameStatePlaying::OnKeyDown(int scancode)
  {
    switch (scancode)
    {
    case SDL_SCANCODE_LEFT : break;
    case SDL_SCANCODE_RIGHT: break;
    case SDL_SCANCODE_ESCAPE: m_engine->PostQuitEvent(); break;
    }
  }

};