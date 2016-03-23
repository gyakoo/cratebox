#pragma once
#include <base/common.h>
#include <base/game.h>
#include <base/fsm.h>
#include <Box2D/Box2D.h>

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
    std::unique_ptr<b2World> m_physics;

    std::array<b2Body*, 50> m_balls;
  };

};