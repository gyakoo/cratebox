#pragma once
#include <common.h>
#include <gametimer.h>
#include <board.h>
#include <player.h>
#include <fsm.h>

namespace PuzzleGame
{

  class Game : public IKeyListener
  {
  public:
    Game(std::shared_ptr<Common> comm);
    ~Game();

    void MainLoop();
    virtual void OnKeyDown(int code);
    std::shared_ptr<Common> GetCommon(){ return m_common; }

  private:
    FSMManager::StateHandle m_hStartingState;
    std::shared_ptr<Common> m_common;
    std::shared_ptr<FSMManager> m_fsm;
  };

};