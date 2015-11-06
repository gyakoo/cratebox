#include <gamestates.h>

namespace PuzzleGame
{

  GameStatePlaying::GameStatePlaying(std::shared_ptr<FSMManager> fsm, std::shared_ptr<Common> comm)
    : m_fsm(fsm), m_common(comm), m_timer(comm, std::chrono::milliseconds(1000))
  {
  }

  void GameStatePlaying::OnEnter()
  {
    m_board = std::make_shared<Board>(m_common, BOARD_DIM_DEFAULT);
    m_player = std::unique_ptr<Player>(new Player(m_common, m_board));
    m_timer.SetPeriod( std::chrono::milliseconds(1000) );
  }

  void GameStatePlaying::OnUpdate()
  {
    if ( m_timer.Update() )
    {
      m_player->DoAction( Player::ROTATE_LEFT );
    }

    m_board->Draw();
  }

  void GameStatePlaying::OnExit()
  {
    m_player = nullptr;
    m_board = nullptr;
  }

  void GameStatePlaying::OnKeyDown(int scancode)
  {
    switch (scancode)
    {
    case SDL_SCANCODE_LEFT : m_player->DoAction(Player::ROTATE_LEFT); break;
    case SDL_SCANCODE_RIGHT: m_player->DoAction(Player::ROTATE_RIGHT); break;
    case SDL_SCANCODE_ESCAPE: m_common->PostQuitEvent(); break;
    }
  }

};