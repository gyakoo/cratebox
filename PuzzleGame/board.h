#pragma once
#include <common.h>

namespace PuzzleGame
{

  class Board
  {
  public:
    enum TileType { TT_NONE, TT_PIECE };
    struct Tile 
    {
      Tile():m_type(TT_NONE){}
      Tile( TileType type, uint32_t time, uint32_t data=0)
        : m_type(type), m_timeCreate(time), m_data(data)
      {}

      TileType m_type;
      uint32_t m_data;
      uint32_t m_timeCreate;
    };
    
  public:
    Board(std::shared_ptr<Common> comm, uint32_t dimension);
    void Update();
    void Draw();
    Tile& GetTile(uint32_t x, uint32_t y);
    Rect GetTileRect(uint32_t x, uint32_t y);
    uint32_t GetDimension(){ return m_dim; }
    void OnPlayerAdvanced(uint32_t plLeft, uint32_t plTop);
    uint32_t GetTimeStamp(){ return m_timeStamp; }
    void SetTimeUntilNext(uint32_t tun){ m_timeUntilNext=tun; } 
  private:
    void DrawTile(const Tile& t, const Rect& r);
    void CreatePiece(uint32_t plLeft, uint32_t plTop);
    bool IsPartOfPlayerLogo(uint32_t x, uint32_t y, uint32_t plLeft, uint32_t plTop);
    Tile RandomTile(uint32_t x, uint32_t y);

  private:
    std::shared_ptr<Common> m_common;
    std::vector< Tile > m_tiles;
    uint32_t m_dim;
    uint32_t m_tileWidth;
    uint32_t m_tileHeight;
    uint32_t m_borderHoriz;
    uint32_t m_borderVert;
    uint32_t m_timeStamp;
    uint32_t m_timeUntilNext;
    std::function<uint32_t (void)>  m_diceColor;
  };
};