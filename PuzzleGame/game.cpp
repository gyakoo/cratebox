#include <game.h>

namespace PuzzleGame
{

Game::Game(std::shared_ptr<Common> comm) 
  : m_common(comm), m_curState(STATE_NONE)
  , m_timer(comm,std::chrono::milliseconds(1000))
{
  m_common->AddKeyListener(this);
}

Game::~Game()
{
  m_common->RemoveKeyListener(this);
}

void Game::MainLoop()
{
  while ( m_common->BeginLoop() )
  {
    StateUpdate(m_curState);
    StateDraw(m_curState);
    m_common->EndLoop();
  }
}
    
void Game::OnKeyDown(int code)
{
  switch(m_curState)
  {
  case STATE_PLAYING:
    switch ( code )
    {
    case SDL_SCANCODE_LEFT : m_player->DoAction(Player::ROTATE_LEFT); break;
    case SDL_SCANCODE_RIGHT: m_player->DoAction(Player::ROTATE_RIGHT); break;
    case SDL_SCANCODE_ESCAPE: m_common->PostQuitEvent(); break;
    }break;
  }
}

void Game::RaiseEvent( GameEvent evt )
{
  switch ( evt )
  {
  case EVENT_PLAY:
    switch (m_curState)
    {
    case STATE_NONE:
      StateLeave(m_curState);
      StateEnter(STATE_PLAYING);
      break;
    }
    break;
  }
}

void Game::StateLeave(GameState st)
{
}

void Game::StateEnter(GameState st)
{
  switch (st)
  {
  case STATE_PLAYING:        
    m_board = std::make_shared<Board>( m_common, BOARD_DIM_DEFAULT );
    m_player = std::make_shared<Player>( m_common, m_board );
    m_timer.SetPeriod( std::chrono::milliseconds(1000) );
    break;
  }
  m_curState = st;
}

void Game::StateUpdate(GameState st)
{
  switch(st)
  {
  case STATE_PLAYING:
    if ( m_timer.Update() )
    {
      m_player->DoAction(Player::ROTATE_LEFT);
    }
    break;
  }
}

void Game::StateDraw(GameState st)
{
  switch(st)
  {
  case STATE_PLAYING:
    m_board->Draw();
    break;
  }
}
};