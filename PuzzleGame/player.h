#pragma once
#include <common.h>
#include <board.h>

namespace PuzzleGame
{
  class Player : public IKeyListener
  {
  public:
    Player(std::shared_ptr<Engine> engine, std::shared_ptr<Board> board);
    ~Player();
    void Update();
    void Draw();

    virtual void OnKeyDown(int scancode);

  private:
    enum LogoTileType { LTT_R=0, LTT_G, LTT_Y, LTT_B };
    void Move(int dx, int dy);
    void RotateLeft();
    void RotateRight();
    bool CanMoveTo(int left, int top);
    void MatchPieces();
    void GetOverlappingPieces(uint32_t left, uint32_t top, std::array<Board::Tile,4>& outPieces);

    uint32_t m_top, m_left;
    std::shared_ptr<Engine> m_engine;
    std::shared_ptr<Board> m_board;
    std::array<LogoTileType,4> m_logoTiles;
  };
};