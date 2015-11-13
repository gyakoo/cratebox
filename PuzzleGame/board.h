#pragma once
#include <common.h>

namespace PuzzleGame
{
  class Board
  {
  public:
    struct Tile 
    {
    };
    typedef std::vector<std::shared_ptr<Tile>> TileCollection;

    
  public:
    Board(std::shared_ptr<Engine> engine, uint32_t twidth, uint32_t theight, uint32_t dimension);
    void Update();
    void Draw();
    std::shared_ptr<Tile> GetTile(uint32_t x, uint32_t y);
    Rect GetTileRect(uint32_t x, uint32_t y);
    uint32_t GetDimension(){ return m_dim; }
    
  private:
    void DrawGrid();

  private:    
    std::shared_ptr<Engine> m_engine;
    TileCollection m_tiles;
    uint32_t m_dim;
    uint32_t m_tileWidth;
    uint32_t m_tileHeight;
    uint32_t m_borderHoriz;
    uint32_t m_borderVert;
    std::unique_ptr<Text> m_label;
    /*std::function<uint32_t (void)>  m_diceColor;
    std::function<bool (void)> m_diceBool;*/
  };
};