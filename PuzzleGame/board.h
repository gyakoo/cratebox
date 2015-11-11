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
        : m_type(type), m_timeCreate(time), m_data(data), m_life(1000)
      {}
      bool IsNone(){return m_type==TT_NONE;}
      bool IsPiece(){ return m_type==TT_PIECE;}

      TileType m_type;
      uint32_t m_data;
      uint32_t m_timeCreate;
      std::chrono::milliseconds m_life;
    };
    
  public:
    Board(std::shared_ptr<Engine> engine, uint32_t twidth, uint32_t theight, uint32_t dimension);
    void Update();
    void Draw();
    Tile& GetTile(uint32_t x, uint32_t y);
    Rect GetTileRect(uint32_t x, uint32_t y);
    uint32_t GetDimension(){ return m_dim; }
    void OnPlayerAdvanced(uint32_t plLeft, uint32_t plTop);
    uint32_t GetTimeStamp(){ return m_timeStamp; }
    void SetTimeUntilNext(uint32_t tun){ m_timeUntilNext=tun; } 
    void MatchPiece(uint32_t plLeft, uint32_t plTop, uint32_t x, uint32_t y);

  private:
    void DrawTile(const Tile& t, const Rect& r);
    void DrawGrid();
    void CreatePiece(uint32_t plLeft, uint32_t plTop);
    bool IsPartOfPlayerLogo(uint32_t x, uint32_t y, uint32_t plLeft, uint32_t plTop);
    Tile RandomTile(uint32_t x, uint32_t y);
    void OnNoMoreSpaceForNewPiece();
    std::vector<std::pair<uint32_t,uint32_t>> GetEmptyTileSlots(uint32_t plLeft, uint32_t plTop);
    void UpdateGUI();

  private:
    std::shared_ptr<Engine> m_engine;
    std::vector< Tile > m_tiles;
    uint32_t m_dim;
    uint32_t m_tileWidth;
    uint32_t m_tileHeight;
    uint32_t m_borderHoriz;
    uint32_t m_borderVert;
    uint32_t m_timeStamp;
    uint32_t m_timeUntilNext;
    std::unique_ptr<Text> m_label;
    std::function<uint32_t (void)>  m_diceColor;
    std::function<bool (void)> m_diceBool;
  };
};