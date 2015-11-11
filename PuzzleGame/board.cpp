#include <board.h>

namespace PuzzleGame
{
  // this class allows to iterate the vector of tiles easily in a for-range loop
  // keeping the x,y information as long as the index and the rect
#pragma warning(disable:4512)
  template<typename T>
  class TileRange
  {
  public:
    struct Iterator
    {
      Iterator(uint32_t x, uint32_t y, uint32_t ti, std::vector<T>& cont, uint32_t tw=0, uint32_t th=0)
        : m_x(x), m_y(y), m_tileNdx(ti), m_container(cont), m_twidth(tw), m_theight(th) { }

      void operator ++()
      {
        uint32_t dim = (uint32_t)std::sqrt(m_container.size());
        ++m_x;
        if ( m_x >= dim )
        {
          m_x = 0;
          ++m_y;
        }
        ++m_tileNdx;
      }
      
      bool operator ==(const Iterator& it) const { return m_tileNdx==it.m_tileNdx; }
      bool operator !=(const Iterator& it) const { return !this->operator==(it); }
      Iterator operator *() const { return *this; }

      uint32_t x()const { return m_x;}
      uint32_t y()const { return m_y;}
      std::pair<uint32_t, uint32_t> xy(){ return std::make_pair(m_x,m_y); }
      T& tile(){ return m_container[m_tileNdx]; }
      const T& tile() const { return m_container[m_tileNdx]; }
      Rect rect(int dx=0, int dy=0) const { return Rect(m_x*m_twidth+dx, m_y*m_theight+dy, m_twidth, m_theight); }

    private:
      std::vector<T>& m_container;
      uint32_t m_x, m_y;
      uint32_t m_twidth, m_theight;
      uint32_t m_tileNdx;
    };

  public:
    TileRange(std::vector<T>& c, uint32_t tw=0, uint32_t th=0) : c(c), tw(tw), th(th) {}
    TileRange& operator =(const TileRange& tr) { return *this; }
    Iterator begin() { return Iterator(0,0,0,c,tw,th); }
    Iterator end() { return Iterator(0,0,c.size(),c,tw,th); }

    std::vector<T>& c;
    uint32_t tw, th;
  };
#pragma warning(default:4512)


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
    auto elapsed = m_engine->GetTimerDelta();
    const auto& zeroms = std::chrono::milliseconds::zero();

    for (auto it : TileRange<Tile>(m_tiles))
    {
      auto& t = it.tile();
      if (t.IsPiece() && t.m_life > zeroms)
      {
        t.m_life -= elapsed;
        if (t.m_life <= zeroms)
        {

        }
      }
    }
  }

  void Board::Draw()
  {
    DrawGrid();

    for (auto it : TileRange<Tile>(m_tiles,m_tileWidth,m_tileHeight))
    {
      auto& t = it.tile();
      auto r = it.rect(m_borderHoriz, m_borderVert);
      DrawTile(t,r);
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

    for (auto it : TileRange<Tile>(m_tiles))
    {
      auto& t = it.tile();
      if ( t.IsNone() && !IsPartOfPlayerLogo(it.x(),it.y(),plLeft,plTop) )
      {
        empties.push_back( it.xy() );
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