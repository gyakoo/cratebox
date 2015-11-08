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

      m_tracks.resize( 8 );
      uint32_t lleft=0, ltop=0;
      GetLogoTileXY(lleft,ltop);
      const uint32_t tracklen = m_dim/2 -1;
      m_tracks[0] = Track(lleft,0,1,tracklen, lleft, 0, 0, 1);
      m_tracks[1] = Track(lleft+1,0,1,tracklen, lleft+1, 0, 0, 1);
      m_tracks[2] = Track(lleft+1,ltop+2,1,tracklen, lleft+1, ltop+1+tracklen, 0, -1);
      m_tracks[3] = Track(lleft,ltop+2,1,tracklen, lleft, ltop+1+tracklen, 0, -1);
      m_tracks[4] = Track(lleft+2,ltop,tracklen,1, lleft+1+tracklen, ltop, -1, 0);
      m_tracks[5] = Track(lleft+2,ltop+1,tracklen,1, lleft+1+tracklen, ltop+1, -1, 0);
      m_tracks[6] = Track(0,ltop,tracklen,1, 0, ltop, 1, 0);
      m_tracks[7] = Track(0,ltop+1,tracklen,1, 0, ltop+1, 1, 0);
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

    void Board::GetLogoTileXY(uint32_t& outX, uint32_t& outY)
    {
      outX = outY = m_dim/2-1;
    }

    Board::Tile& Board::GetTile(uint32_t x, uint32_t y)
    {
      if ( x >= m_dim || y >= m_dim )
        throw std::exception("invalid tile coords");
      return m_tiles[ m_dim*y + x ];
    }

    bool Board::IsTrackEmpty(int track)
    {
      auto tiles = GetTilesInRect( m_tracks[track].m_rect, SMOVING);  
      return tiles.empty();
    }

    int Board::PickRandomTrack()
    {
      // -1 if track is occupied
      std::array<int,8> tracks;
      for ( int i = 0; i < 8; ++i )
        if ( IsTrackEmpty(i) )
          tracks[i]=i;
        else
          tracks[i]=-1;

      // random sort
      std::random_shuffle(tracks.begin(), tracks.end());

      // find first available
      for ( int i = 0; i < 4; ++i )
        if (tracks[i]!=-1) 
          return tracks[i];
      return -1;
    }
    
    Board::TileType Board::PickRandomTileType()
    {
      std::array<Board::TileType, 4> colors={TRED, TGREEN, TBLUE, TYELLOW};
      std::random_shuffle(colors.begin(), colors.end());
      return colors[0];
    }

    std::vector<std::pair<uint32_t,uint32_t>> Board::GetTilesInRect( const Rect& rect, TileStatus status)
    {
      std::vector<std::pair<uint32_t,uint32_t>> tiles;
      for ( int j = rect.y; j<rect.y+rect.height; ++j )
      {
        for ( int i = rect.x; i<rect.x+rect.width; ++i )
        {
          if ( GetTile(i,j).m_status == status )
            tiles.push_back(std::make_pair(i,j));
        }
      }
      return tiles;
    }

    void Board::GetTrackStart(uint32_t track, int& startx, int& starty)
    {
      const auto& t = m_tracks[track];
      startx = t.m_startX;
      starty = t.m_startY;
    }

    void Board::GetTrackDir(uint32_t track, int& dirx, int& diry)
    {
      const auto& t = m_tracks[track];
      dirx = t.m_dirX;
      diry = t.m_dirY;
    }
};