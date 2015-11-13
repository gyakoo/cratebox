#pragma once
#include <common.h>
#include <gametimer.h>
#include <board.h>
#include <fsm.h>

namespace PuzzleGame
{

  class Game : public IKeyListener
  {
  public:
    Game(std::shared_ptr<Engine> engine);
    ~Game();

    void MainLoop();
    virtual void OnKeyDown(int code);
    std::shared_ptr<Engine> GetEngine(){ return m_engine; }

  private:
    FSMManager::StateHandle m_hStartingState;
    std::shared_ptr<Engine> m_engine;
    std::shared_ptr<FSMManager> m_fsm;
  };

};