#pragma once
#include <common.h>

namespace PuzzleGame
{

  class Game : public IKeyListener
  {
  public:
    enum GameState { STATE_NONE, STATE_PLAYING };
    enum GameEvent { EVENT_PLAY };
  
  public:
    Game(std::shared_ptr<Common> comm);
    ~Game();

    void MainLoop();
    virtual void OnKeyDown(int code);
    void RaiseEvent( GameEvent evt );

  private:
    void StateLeave(GameState st);
    void StateEnter(GameState st);
    void StateUpdate(GameState st);
    void StateDraw(GameState st);

  private:
    GameTimer m_timer;
    GameState m_curState;
    std::shared_ptr<Common> m_common;
    std::shared_ptr<Board> m_board;
    std::shared_ptr<Player> m_player;
  };

};