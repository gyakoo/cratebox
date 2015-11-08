#pragma once
#include <common.h>

namespace PuzzleGame
{

  class Board
  {
  public:
    enum TileType { TNONE, TRED, TGREEN, TBLUE, TYELLOW };
    enum TileStatus { SNONE, SFIXED, SMOVING };
    struct Tile 
    {
      Tile(TileType t=TNONE, TileStatus s=SNONE)
        : m_type(t), m_status(s)
      {}

      TileType m_type;
      TileStatus m_status;
    };
    
  public:
    Board(std::shared_ptr<Common> comm, uint32_t dimension);
    void Update();
    void Draw();
    void RotateLeft();
    void RotateRight();
    Tile& GetTile(uint32_t x, uint32_t y);
    void GetLogoTileXY(uint32_t& outX, uint32_t& outY);
    int PickRandomTrack();
    TileType PickRandomTileType();
    void GetTrackStart(uint32_t track, int& startx, int& starty);
    void GetTrackDir(uint32_t track, int& dirx, int& diry);

  private:
    void DrawTile(const Tile& t, const Rect& r);
    template<bool LEFT>
    void LogoRotate();
    void LogoReset();
    uint32_t GetTopLeftTileNdx();
    bool IsTrackEmpty(int track);
    std::vector<std::pair<uint32_t,uint32_t>> GetTilesInRect( const Rect& rect, TileStatus status);

  private:
    struct Track
    {
      Track(){}
      Track(int x, int y, int w, int h, int sx, int sy, int dx, int dy)
        :m_rect(x,y,w,h), m_startX(sx), m_startY(sy), m_dirX(dx), m_dirY(dy)
      {
      }

      int m_startX, m_startY;
      int m_dirX, m_dirY;
      Rect m_rect;
    };

    std::shared_ptr<Common> m_common;
    std::vector< Tile > m_tiles;
    std::vector< Track > m_tracks;
    uint32_t m_dim;
    uint32_t m_tileWidth;
    uint32_t m_tileHeight;
    uint32_t m_borderHoriz;
    uint32_t m_borderVert;
  };
};