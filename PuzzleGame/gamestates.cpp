#include <gamestates.h>
#include <stdarg.h>

namespace Game
{

  GameStatePlaying::GameStatePlaying(std::shared_ptr<FSMManager> fsm, std::shared_ptr<Engine> engine)
    : m_fsm(fsm), m_engine(engine), m_timer(engine)
  {
  }

  void GameStatePlaying::OnEnter()
  {
  }

  void GameStatePlaying::OnUpdate()
  {
    m_timer.Update();
  }

  void GameStatePlaying::OnExit()
  {
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