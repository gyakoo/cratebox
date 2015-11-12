#pragma once
#include <common.h>
#include <board.h>

namespace PuzzleGame
{
  //class Player : public IKeyListener
  //{
  //public:
  //  enum LogoTileType { LTT_R=0, LTT_G, LTT_Y, LTT_B };
  //  Player(std::shared_ptr<Engine> engine, std::shared_ptr<Board> board);
  //  ~Player();
  //  void Update();
  //  void Draw();
  //  uint32_t GetTop(){ return m_top; }
  //  uint32_t GetLeft(){ return m_left; }
  //  void GetPosOf(LogoTileType tileType, uint32_t& outX, uint32_t& outY);
  //  virtual void OnKeyDown(int scancode);
  //  bool IsPiece(LogoTileType tileType, uint32_t x, uint32_t y); 
  //  void BrokenPiece(LogoTileType tileType);

  //private:
  //  void Move(int dx, int dy);
  //  void RotateLeft();
  //  void RotateRight();
  //  bool CanMoveTo(int left, int top);
  //  void MatchPieces();
  //  void GetOverlappingPieces(uint32_t left, uint32_t top, std::array<Board::Tile,4>& outPieces);

  //  uint32_t m_top, m_left;
  //  std::shared_ptr<Engine> m_engine;
  //  std::shared_ptr<Board> m_board;
  //  std::array<LogoTileType,4> m_logoTiles;
  //};
};