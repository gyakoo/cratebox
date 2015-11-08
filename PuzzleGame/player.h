#pragma once
#include <common.h>
#include <board.h>

namespace PuzzleGame
{
  class Player
  {
  public:
    Player(std::shared_ptr<Common> comm, std::shared_ptr<Board> board);

    void RotateLeft();
    void RotateRight();
    void HitOk(int x, int y);
    void HitWrong(int x, int y);

  private:
    std::shared_ptr<Common> m_common;
    std::shared_ptr<Board> m_board;
  };

};