#pragma once
#include <common.h>
#include <game.h>
#include <fsm.h>

namespace Game
{
  enum GameEvents
  {
    GAMEVENT_GO_PLAY
  };

  class GameStatePlaying : public FSMState
  {
  public:
    GameStatePlaying(std::shared_ptr<FSMManager> fsm, std::shared_ptr<Engine> engine);
    virtual void OnEnter();
    virtual void OnUpdate();
    virtual void OnExit();
    virtual void OnKeyDown(int scancode);

  private:
    GameTimer m_timer;
    std::shared_ptr<FSMManager> m_fsm;
    std::shared_ptr<Engine> m_engine;
  };

};