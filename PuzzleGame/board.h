#pragma once
#include <common.h>

namespace PuzzleGame
{

  class Board
  {
  public:
    enum TileType { TNONE, TRED, TGREEN, TBLUE, TYELLOW };
    enum TileStatus { SNONE, SFIXED, SATTACHED, SMOVING };
    struct Tile 
    {
      Tile(TileType t=TNONE, TileStatus s=SNONE)
        : m_type(t), m_status(s), m_dirX(0), m_dirY(0)
      {}

      TileType m_type;
      TileStatus m_status;
      int m_dirX, m_dirY;
    };
    
  public:
    Board(std::shared_ptr<Common> comm, uint32_t dimension);
    void Update();
    void Draw();
    void RotateLeft();
    void RotateRight();

  private:
    void DrawTile(const Tile& t, const Rect& r);
    template<bool LEFT>
    void LogoRotate();
    void LogoReset();
    uint32_t GetTopLeftTileNdx();
  
  private:
    std::shared_ptr<Common> m_common;
    std::vector< Tile > m_tiles;
    uint32_t m_dim;
    uint32_t m_tileWidth;
    uint32_t m_tileHeight;
    uint32_t m_borderHoriz;
    uint32_t m_borderVert;
  };
};