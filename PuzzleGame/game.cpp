#include <game.h>
#include <gamestates.h>

namespace PuzzleGame
{

Game::Game(std::shared_ptr<Engine> engine) 
  : m_engine(engine), m_fsm( std::make_shared<FSMManager>() )
{
  m_engine->AddKeyListener(this);
  m_hStartingState = m_fsm->AddState( std::make_shared<GameStatePlaying>(m_fsm,m_engine) );
}

Game::~Game()
{
  m_engine->RemoveKeyListener(this);
}

void Game::MainLoop()
{
  m_fsm->Start( m_hStartingState );
  while ( m_engine->BeginLoop() )
  {
    m_fsm->Update();  
    m_engine->EndLoop();
  }
}
    
void Game::OnKeyDown(int code)
{
  auto st = m_fsm->GetState( m_fsm->GetCurrentState() );
  st->OnKeyDown(code);
}

};