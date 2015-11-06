#include <common.h>
#include <game.h>
#include <gametimer.h>

namespace PuzzleGame
{
  enum PuzzleGameConstants
  {
    LOGO_N = 4,
    BOARD_DIM_DEFAULT = 14
  };

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
    Board(std::shared_ptr<Common> comm, uint32_t dimension)
      : m_common(comm), m_dim(dimension)
    {
      if ( dimension <= 2 || dimension%2!=0 )
        throw std::exception("Invalid board dimension, has to be >2 and even");
      m_tileWidth = m_common->GetWidth() / dimension;
      m_tileHeight = m_common->GetHeight() / dimension;
      m_borderHoriz = (m_common->GetWidth() - ( m_tileWidth * m_dim )) / 2;
      m_borderVert = (m_common->GetHeight() - ( m_tileHeight * m_dim ) ) / 2;
      m_tiles.resize( dimension * dimension, Tile() );
      LogoReset();
    }

    void Update()
    {
    }

    void Draw()
    {
      Rect r(m_borderHoriz, m_borderVert, m_tileWidth, m_tileHeight);
      auto x = 0u;
      for ( auto t : m_tiles )
      {
        DrawTile(t, r);
        ++x;
        if ( x >= m_dim )
        {
          r.x = m_borderHoriz;
          r.y += m_tileHeight;
          x = 0;
        }
        else
        {
          r.x += m_tileWidth;
        }
      }
    }

    void RotateLeft() { LogoRotate<true>(); }
    void RotateRight(){ LogoRotate<false>(); }

  private:
    void DrawTile(const Tile& t, const Rect& r)
    {
      Rect dr=r;
      dr.Deflate(6,6);
      Color c;
      switch ( t.m_type )
      {
      case TRED: c=Colors::MSRED; break;
      case TBLUE: c=Colors::MSBLUE; break;
      case TGREEN: c=Colors::MSGREEN; break;
      case TYELLOW: c=Colors::MSYELLOW; break;
      default: c.a = 0;
      }

      if ( c.a != 0 )
      {
        m_common->FillRect(dr, c); 
      }
    }
    
    template<bool LEFT>
    void LogoRotate()
    {
      auto tl = GetTopLeftTileNdx();
      auto tr = tl+1;
      auto br = tr+m_dim;
      auto bl = br-1;
      if ( LEFT )
      {
        Tile first = m_tiles[tl];
        m_tiles[tl]=m_tiles[tr];
        m_tiles[tr]=m_tiles[br];
        m_tiles[br]=m_tiles[bl];
        m_tiles[bl]=first;
      }
      else
      {
        Tile last = m_tiles[bl];
        m_tiles[bl] = m_tiles[br];
        m_tiles[br] = m_tiles[tr];
        m_tiles[tr] = m_tiles[tl];
        m_tiles[tl] = last;
      }
    }

    void LogoReset()
    {
      auto x = GetTopLeftTileNdx();
      m_tiles[x] = Tile(TRED, SFIXED);
      m_tiles[x+1] = Tile(TGREEN, SFIXED);
      m_tiles[x+m_dim]= Tile(TBLUE, SFIXED);
      m_tiles[x+m_dim+1] = Tile(TYELLOW, SFIXED);
    }

    uint32_t GetTopLeftTileNdx()
    { 
      return (m_dim / 2 - 1)*( m_dim + 1); 
    }

    std::shared_ptr<Common> m_common;
    std::vector< Tile > m_tiles;
    uint32_t m_dim;
    uint32_t m_tileWidth;
    uint32_t m_tileHeight;
    uint32_t m_borderHoriz;
    uint32_t m_borderVert;
  };

  class Player
  {
  public:
    enum ActionType { ROTATE_LEFT, ROTATE_RIGHT };
    Player(std::shared_ptr<Common> comm, std::shared_ptr<Board> board)
      : m_common(comm), m_board(board)
    {
    }

    void DoAction( ActionType at )
    {
      switch (at)
      {
      case ROTATE_LEFT: m_board->RotateLeft(); break;
      case ROTATE_RIGHT: m_board->RotateRight(); break;
      }
    }

  private:
    std::shared_ptr<Common> m_common;
    std::shared_ptr<Board> m_board;
  };

  };


int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
  using namespace PuzzleGame;
  auto comm = std::make_shared<Common>( 800, 600, "Puzzle Game", false );
  Game game(comm);
  game.RaiseEvent(Game::EVENT_PLAY);
  game.MainLoop();
  
  
  return S_OK;
}

/*
Game
  Common
  Board
  Player

*/