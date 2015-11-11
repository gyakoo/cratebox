#include <board.h>

namespace PuzzleGame
{
  Board::Board(std::shared_ptr<Engine> engine, uint32_t dimension, uint32_t twidth, uint32_t theight)
      : m_engine(engine), m_dim(dimension), m_timeStamp(0), m_timeUntilNext(2)
      , m_tileWidth(twidth), m_tileHeight(theight)
  {
    if ( dimension <= 2 )
      throw std::exception("Invalid board dimension, has to be >2");
    m_borderHoriz = (m_engine->GetWidth() - ( m_tileWidth * m_dim )) / 2;
    m_borderVert = (m_engine->GetHeight() - ( m_tileHeight * m_dim ) ) / 2;
    m_tiles.resize( dimension * dimension );
    std::default_random_engine  generator;
    m_diceColor = std::bind( std::uniform_int_distribution<uint32_t>(0,3), generator );
    m_diceBool = std::bind( std::bernoulli_distribution(0.5), generator );
    m_label = std::unique_ptr<Text>(new Text(m_engine->GetFont("data/OpenSans-Bold.ttf",24), "hey", Colors::YELLOW));
  }

  void Board::Update()
  {
  }

  void Board::Draw()
  {
    DrawGrid();
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

  void Board::DrawGrid()
  {
    uint32_t x0=m_borderHoriz;
    uint32_t y1=m_borderVert+m_tileHeight*m_dim;
    for (uint32_t x = 0; x <= m_dim; ++x, x0+=m_tileWidth)
      m_engine->DrawLine( x0, m_borderVert, x0, y1, Colors::WHITE );

    uint32_t y0=m_borderVert;
    uint32_t x1=m_borderHoriz+m_tileWidth*m_dim;
    for (uint32_t y = 0; y <= m_dim; ++y, y0+=m_tileHeight)
      m_engine->DrawLine( m_borderHoriz, y0, x1, y0, Colors::WHITE );
  }

  void Board::DrawTile(const Tile& t, const Rect& r)
  {
    static std::array<Color,4> s_colors={
      Colors::MSRED, Colors::MSGREEN, 
      Colors::MSYELLOW, Colors::MSBLUE };
    Rect dr=r;

    dr.Translate(1,1);
    dr.Deflate(43,43);

    Color c(0,0,0,0);
    switch ( t.m_type )
    {
    case TT_PIECE: c=s_colors[ t.m_data ]; break;
    }

    if ( c.a != 0 )
    {
      m_engine->FillRect(dr, c); 
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
      CreatePiece(plLeft, plTop);
    }
    UpdateGUI();
  }

  void Board::CreatePiece(uint32_t plLeft, uint32_t plTop)
  {
    // gather all available spaces
    auto empties = GetEmptyTileSlots(plLeft, plTop);
    
    // random sort
    if ( !empties.empty() )
    {
      std::random_shuffle( empties.begin(), empties.end() );
      auto rndPos = *empties.begin();
      m_tiles[ rndPos.second*m_dim+rndPos.first ] = RandomTile( rndPos.first, rndPos.second );
      m_timeUntilNext = 2;
    }
    else
    {
      OnNoMoreSpaceForNewPiece();
    }
    UpdateGUI();
  }

  std::vector<std::pair<uint32_t,uint32_t>> Board::GetEmptyTileSlots(uint32_t plLeft, uint32_t plTop)
  {
    std::vector<std::pair<uint32_t,uint32_t>> empties;
    empties.reserve( m_tiles.size() );
    for ( uint32_t y = 0; y < m_dim; ++y )
    {
      for ( uint32_t x = 0; x < m_dim; ++x )
      {
        auto& t = m_tiles[ y*m_dim+x ];
        if ( t.IsNone() && !IsPartOfPlayerLogo(x,y,plLeft,plTop) )
        {
          empties.push_back( std::make_pair(x,y) );
        }
      }
    }
    return empties;
  }

  bool Board::IsPartOfPlayerLogo(uint32_t x, uint32_t y, uint32_t plLeft, uint32_t plTop)
  {
    return x >= plLeft && x <= (plLeft+1) && y >= plTop && y <= (plTop+1);
  }

  void Board::OnNoMoreSpaceForNewPiece()
  {
    m_label->SetText( "No more" );
  }

  Board::Tile Board::RandomTile(uint32_t x, uint32_t y)
  {
    // get adjacent pieces
    std::array<std::pair<uint32_t,uint32_t>,4> adjs;
    uint32_t c=0;
    if ( y>0 && GetTile(x,y-1).IsPiece() ) adjs[c++]=std::make_pair(x,y-1);
    if ( y<m_dim-1 && GetTile(x,y+1).IsPiece() ) adjs[c++]=std::make_pair(x,y+1);
    if ( x>0 && GetTile(x-1,y).IsPiece() ) adjs[c++]=std::make_pair(x-1,y);
    if ( x<m_dim-1 && GetTile(x+1,y).IsPiece() ) adjs[c++]=std::make_pair(x+1,y);

    // if any adjacent piece, select adjacent color to match with logo
    Tile retTile(TT_PIECE, m_timeStamp, m_diceColor());
    if ( c>0 )
    {
      std::random_shuffle(adjs.begin(), adjs.begin()+c);
      auto& pos=*adjs.begin();
      int curCol = (int)GetTile(pos.first, pos.second).m_data;
      if ( (pos.first < x || pos.second < y) && m_diceBool() )
        retTile.m_data = (curCol-1+4)%4;
      else
        retTile.m_data = (curCol+1)%4;
    }
    return retTile;
  }

  void Board::MatchPiece(uint32_t plLeft, uint32_t plTop, uint32_t x, uint32_t y)
  {
    m_tiles[y*m_dim+x].m_type = TT_NONE;
    auto empties = GetEmptyTileSlots(m_dim,m_dim);
    // no more pieces left
    if ( empties.size() == (m_dim*m_dim) )
    {
      CreatePiece(plLeft, plTop);
    }
  }

  void Board::UpdateGUI()
  {
    m_label->SetText( StringUtils::From((int)m_timeUntilNext) );
  }
};