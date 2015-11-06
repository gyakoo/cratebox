#include <game.h>
#include <gamestates.h>

namespace PuzzleGame
{

Game::Game(std::shared_ptr<Common> comm) 
  : m_common(comm), m_fsm( std::make_shared<FSMManager>() )
{
  m_common->AddKeyListener(this);
  m_hStartingState = m_fsm->AddState( std::make_shared<GameStatePlaying>(m_fsm,m_common) );
}

Game::~Game()
{
  m_common->RemoveKeyListener(this);
}

void Game::MainLoop()
{
  m_fsm->Start( m_hStartingState );
  while ( m_common->BeginLoop() )
  {
    m_fsm->Update();  
    m_common->EndLoop();
  }
}
    
void Game::OnKeyDown(int code)
{
  auto st = m_fsm->GetState( m_fsm->GetCurrentState() );
  st->OnKeyDown(code);
}

};