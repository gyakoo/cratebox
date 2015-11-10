#include <gamestates.h>
#include <stdarg.h>

namespace PuzzleGame
{

  GameStatePlaying::GameStatePlaying(std::shared_ptr<FSMManager> fsm, std::shared_ptr<Common> comm)
    : m_fsm(fsm), m_common(comm), m_timer(comm)
  {
  }

  void GameStatePlaying::OnEnter()
  {
    m_board = std::make_shared<Board>(m_common, BOARD_DIM_DEFAULT, 96, 96);
    m_player = std::unique_ptr<Player>(new Player(m_common, m_board));
    //m_timer.AddCallback( std::chrono::milliseconds(2500), std::bind(&GameStatePlaying::OnCreatePiece, this) );
    m_font = std::make_shared<Font>(m_common, "OpenSans-Bold.ttf", 24);
  }

  void GameStatePlaying::OnUpdate()
  {
    m_timer.Update();
    m_player->Update();

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
    case SDL_SCANCODE_ESCAPE: m_common->PostQuitEvent(); break;
    }
  }

};