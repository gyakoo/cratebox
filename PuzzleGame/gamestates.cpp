#include <gamestates.h>
#include <stdarg.h>
#include <board.h>
#include <player.h>

namespace PuzzleGame
{

  GameStatePlaying::GameStatePlaying(std::shared_ptr<FSMManager> fsm, std::shared_ptr<Engine> engine)
    : m_fsm(fsm), m_engine(engine), m_timer(engine)
  {
  }

  void GameStatePlaying::OnEnter()
  {
    m_board = std::make_shared<Board>(m_engine, BOARD_DIM_DEFAULT, 96, 96);
    m_player = std::make_shared<Player>(m_engine, m_board);
    //m_timer.AddCallback( std::chrono::milliseconds(2500), std::bind(&GameStatePlaying::OnCreatePiece, this) );
  }

  void GameStatePlaying::OnUpdate()
  {
    m_timer.Update();
    m_player->Update();
    m_board->Update(m_player);

    m_board->Draw();
    m_player->Draw();
  }

  void GameStatePlaying::OnExit()
  {
    m_player = nullptr;
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