#pragma once
#include <common.h>

namespace PuzzleGame
{
  class Entity;
  class Player; 

  class Board
  {
  public:
    enum TileType { TT_NONE, TT_PIECE, TT_BLOCKED, TT_FILLED };
    enum PieceType { PT_RED, PT_GREEN, PT_YELLOW, PT_BLUE };
    struct Tile 
    {
      Tile():m_type(TT_NONE){}
      Tile( TileType type, uint32_t time, uint32_t data=0)
        : m_type(type), m_timeCreate(time), m_data(data)
      {
        SetLife(std::chrono::seconds(3));
      }
      bool IsNone() const {return m_type==TT_NONE;}
      bool IsPiece() const { return m_type==TT_PIECE;}
      bool IsBlocked() const { return m_type==TT_BLOCKED; }
      bool IsFilled() const { return m_type==TT_FILLED; }
      void SetLife(std::chrono::milliseconds ms) { m_life = m_totalLife = ms; }
      void SetBlocked(){ m_type=TT_BLOCKED; }
      void SetNone() { m_type = TT_NONE; }
      void SetFilled(){ m_type = TT_FILLED; }
      
      float GetLifeNormal() const 
      { 
        const float n = (float)m_life.count()/m_totalLife.count(); 
        return n<0.0f ? 0.0f : n;
      }

      TileType m_type;
      uint32_t m_data;
      uint32_t m_timeCreate;
      std::chrono::milliseconds m_life;
      std::chrono::milliseconds m_totalLife;
    };

    
  public:
    Board(std::shared_ptr<Engine> engine, uint32_t twidth, uint32_t theight, uint32_t dimension);
    void Update(std::shared_ptr<Player> player);
    void Draw();
    Tile& GetTile(uint32_t x, uint32_t y);
    Rect GetTileRect(uint32_t x, uint32_t y);
    uint32_t GetDimension(){ return m_dim; }
    void OnPlayerAdvanced(uint32_t plLeft, uint32_t plTop);
    uint32_t GetTimeStamp(){ return m_timeStamp; }
    void SetTimeUntilNext(int32_t tun){ m_timeUntilNext=tun; } 
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
    void OnPieceTimeUp(Tile& t, uint32_t x, uint32_t y);

  private:
    std::shared_ptr<Engine> m_engine;
    std::vector< Tile > m_tiles;
    std::vector< std::shared_ptr<Entity> > m_entities;
    uint32_t m_dim;
    uint32_t m_tileWidth;
    uint32_t m_tileHeight;
    uint32_t m_borderHoriz;
    uint32_t m_borderVert;
    uint32_t m_timeStamp;
    int32_t  m_timeUntilNext;
    std::unique_ptr<Text> m_label;
    std::function<uint32_t (void)>  m_diceColor;
    std::function<bool (void)> m_diceBool;
  };
};