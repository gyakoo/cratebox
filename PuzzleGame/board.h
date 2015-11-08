#pragma once
#include <common.h>

namespace PuzzleGame
{

  class Board
  {
  public:
    struct Tile 
    {
      Tile()
      {}
    };
    
  public:
    Board(std::shared_ptr<Common> comm, uint32_t dimension);
    void Update();
    void Draw();
    Tile& GetTile(uint32_t x, uint32_t y);
    Rect GetTileRect(uint32_t x, uint32_t y);
    uint32_t GetDimension(){ return m_dim; }

  private:
    void DrawTile(const Tile& t, const Rect& r);

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