#include <player.h>

namespace PuzzleGame
{
  Player::Player(std::shared_ptr<Common> comm, std::shared_ptr<Board> board)
    : m_common(comm), m_board(board)
  {
  }

  void Player::RotateLeft()
  {
    m_board->RotateLeft();
  }

  void Player::RotateRight()
  {
    m_board->RotateRight();
  }

  void Player::HitOk(int x, int y)
  {

  }

  void Player::HitWrong(int x, int y)
  {

  }
};