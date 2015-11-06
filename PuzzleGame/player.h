#pragma once
#include <common.h>
#include <board.h>

namespace PuzzleGame
{
  class Player
  {
  public:
    enum ActionType { ROTATE_LEFT, ROTATE_RIGHT };
    Player(std::shared_ptr<Common> comm, std::shared_ptr<Board> board);

    void DoAction( ActionType at );

  private:
    std::shared_ptr<Common> m_common;
    std::shared_ptr<Board> m_board;
  };

};