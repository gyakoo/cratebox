#include <board.h>

namespace PuzzleGame
{
  Board::Board(std::shared_ptr<Common> comm, uint32_t dimension)
      : m_common(comm), m_dim(dimension), m_timeStamp(0), m_timeUntilNext(3)
    {
      if ( dimension <= 2 )
        throw std::exception("Invalid board dimension, has to be >2");
      m_tileWidth = m_common->GetWidth() / dimension;
      m_tileHeight = m_common->GetHeight() / dimension;
      m_borderHoriz = (m_common->GetWidth() - ( m_tileWidth * m_dim )) / 2;
      m_borderVert = (m_common->GetHeight() - ( m_tileHeight * m_dim ) ) / 2;
      m_tiles.resize( dimension * dimension );
      std::default_random_engine  generator;
      std::uniform_int_distribution<uint32_t> distri(0,3);
      m_diceColor = std::bind( distri, generator );
      m_label = std::unique_ptr<Text>(new Text(m_common->GetDefaultFont(), "hey", Colors::YELLOW));
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
      m_label->Draw(10,10);
    }

    void Board::DrawTile(const Tile& t, const Rect& r)
    {
      Rect dr=r;
      m_common->DrawRect( r, Colors::WHITE ); 

      dr.Deflate(6,6);

      Color c(0,0,0,0);
      static std::array<Color,4> s_colors={Colors::MSRED, Colors::MSGREEN, Colors::MSYELLOW, Colors::MSBLUE };
      switch ( t.m_type )
      {
      case TT_PIECE: c=s_colors[ t.m_data ]; break;
      }

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

    void Board::OnPlayerAdvanced(uint32_t plLeft, uint32_t plTop)
    {
      ++m_timeStamp;
      --m_timeUntilNext;
      if ( m_timeUntilNext == 0 )
      {
        m_timeUntilNext = 3;
        CreatePiece(plLeft, plTop);
      }
      m_label->SetText( StringUtils::From((int)m_timeUntilNext) );
    }

    void Board::CreatePiece(uint32_t plLeft, uint32_t plTop)
    {
      // gather all available spaces
      std::vector< std::pair<uint32_t, uint32_t> > empties;
      empties.reserve( m_tiles.size() );
      for ( uint32_t y = 0; y < m_dim; ++y )
      {
        for ( uint32_t x = 0; x < m_dim; ++x )
        {
          auto& t = m_tiles[ y*m_dim+x ];
          if ( t.m_type == TT_NONE && !IsPartOfPlayerLogo(x,y,plLeft,plTop) )
          {
            empties.push_back( std::make_pair(x,y) );
          }
        }
      }

      // random sort
      std::random_shuffle( empties.begin(), empties.end() );
      auto rndPos = *empties.begin();
      m_tiles[ rndPos.second*m_dim+rndPos.first ] = RandomTile( rndPos.first, rndPos.second );
    }

    bool Board::IsPartOfPlayerLogo(uint32_t x, uint32_t y, uint32_t plLeft, uint32_t plTop)
    {
      return x >= plLeft && x <= (plLeft+1) && y >= plTop && y <= (plTop+1);
    }

    Board::Tile Board::RandomTile(uint32_t x, uint32_t y)
    {
      return Tile(TT_PIECE, m_timeStamp, m_diceColor());    
    }

    void Board::MatchPiece(uint32_t x, uint32_t y)
    {
      m_tiles[y*m_dim+x].m_type = TT_NONE;
    }
};