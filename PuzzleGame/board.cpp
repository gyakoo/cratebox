#include <board.h>

namespace PuzzleGame
{
  Board::Board(std::shared_ptr<Common> comm, uint32_t dimension)
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

    void Board::Update()
    {
    }

    void Board::Draw()
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

    void Board::RotateLeft() { LogoRotate<true>(); }
    void Board::RotateRight(){ LogoRotate<false>(); }

    void Board::DrawTile(const Tile& t, const Rect& r)
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
    void Board::LogoRotate()
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

    void Board::LogoReset()
    {
      auto x = GetTopLeftTileNdx();
      m_tiles[x] = Tile(TRED, SFIXED);
      m_tiles[x+1] = Tile(TGREEN, SFIXED);
      m_tiles[x+m_dim]= Tile(TBLUE, SFIXED);
      m_tiles[x+m_dim+1] = Tile(TYELLOW, SFIXED);
    }

    uint32_t Board::GetTopLeftTileNdx()
    { 
      return (m_dim / 2 - 1)*( m_dim + 1); 
    }

};