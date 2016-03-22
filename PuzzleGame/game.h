#pragma once
#include <common.h>
#include <gametimer.h>
#include <fsm.h>

namespace Game
{

  class GameApp : public IKeyListener
  {
  public:
    GameApp(std::shared_ptr<Engine> engine);
    ~GameApp();

    void MainLoop();
    virtual void OnKeyDown(int code);
    std::shared_ptr<Engine> GetEngine(){ return m_engine; }

  private:
    std::shared_ptr<Engine> m_engine;

    FSMManager::StateHandle m_hStartingState;
    std::shared_ptr<FSMManager> m_fsm;
  };

};