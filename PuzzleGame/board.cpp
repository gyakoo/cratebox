#include <board.h>

namespace PuzzleGame
{
  Board::Board(std::shared_ptr<Common> comm, uint32_t dimension)
      : m_common(comm), m_dim(dimension)
    {
      if ( dimension <= 2 )
        throw std::exception("Invalid board dimension, has to be >2");
      m_tileWidth = m_common->GetWidth() / dimension;
      m_tileHeight = m_common->GetHeight() / dimension;
      m_borderHoriz = (m_common->GetWidth() - ( m_tileWidth * m_dim )) / 2;
      m_borderVert = (m_common->GetHeight() - ( m_tileHeight * m_dim ) ) / 2;
      m_tiles.resize( dimension * dimension, Tile() );
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

    void Board::DrawTile(const Tile& t, const Rect& r)
    {
      Rect dr=r;
      m_common->DrawRect( r, Colors::WHITE ); 

      dr.Deflate(6,6);
      Color c(0,0,0,0);
      if ( c.a != 0 )
      {
        m_common->FillRect(dr, c); 
      }
    }
    
    Board::Tile& Board::GetTile(uint32_t x, uint32_t y)
    {
      if ( x >= m_dim || y >= m_dim )
        throw std::exception("invalid tile coords");
      return m_tiles[ m_dim*y + x ];
    }

    Rect Board::GetTileRect(uint32_t x, uint32_t y)
    {
      return Rect (
        m_borderHoriz + x*m_tileWidth,
        m_borderVert + y*m_tileHeight,
        m_tileWidth, m_tileHeight );
    }
};