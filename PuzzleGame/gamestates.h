#pragma once
#include <common.h>
#include <game.h>
#include <fsm.h>

namespace PuzzleGame
{
  enum GameEvents
  {
    GAMEVENT_GO_PLAY
  };

  class GameStatePlaying : public FSMState
  {
  public:
    GameStatePlaying(std::shared_ptr<FSMManager> fsm, std::shared_ptr<Common> comm);
    virtual void OnEnter();
    virtual void OnUpdate();
    virtual void OnExit();
    virtual void OnKeyDown(int scancode);

  private:
    GameTimer m_timer;
    std::shared_ptr<FSMManager> m_fsm;
    std::shared_ptr<Common> m_common;
    std::shared_ptr<Board> m_board;
    std::unique_ptr<Player> m_player;
    std::shared_ptr<Font> m_font;
  };

};