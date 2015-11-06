#include <player.h>

namespace PuzzleGame
{
  Player::Player(std::shared_ptr<Common> comm, std::shared_ptr<Board> board)
    : m_common(comm), m_board(board)
  {
  }

  void Player::DoAction( ActionType at )
  {
    switch (at)
    {
    case ROTATE_LEFT: m_board->RotateLeft(); break;
    case ROTATE_RIGHT: m_board->RotateRight(); break;
    }
  }

};