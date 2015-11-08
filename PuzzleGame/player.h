#pragma once
#include <common.h>
#include <board.h>

namespace PuzzleGame
{
  class Player : public IKeyListener
  {
  public:
    Player(std::shared_ptr<Common> comm, std::shared_ptr<Board> board);
    ~Player();
    void Update();
    void Draw();

    virtual void OnKeyDown(int scancode);

  private:
    enum LogoTileType { LTT_R=0, LTT_G, LTT_B, LTT_Y };
    void Move(int dx, int dy);
    template<bool LEFT> void Rotate();
    bool CanMoveTo(int left, int top);

    uint32_t m_top, m_left;
    std::shared_ptr<Common> m_common;
    std::shared_ptr<Board> m_board;
    std::array<LogoTileType,4> m_logoTiles;
  };

};