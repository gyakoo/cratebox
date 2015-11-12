#include <board.h>
#include <entities.h>

namespace PuzzleGame
{
  // this class allows to iterate the vector of tiles easily in a for-range loop
  // keeping the x,y information as long as the index and the rect
#pragma warning(disable:4512)  
  class TileRange
  {
  public:
    struct Iterator
    {
      Iterator(uint32_t x, uint32_t y, uint32_t ti, Board::TileCollection& cont, uint32_t tw=0, uint32_t th=0)
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
      std::shared_ptr<Board::Tile> tile(){ return m_container[m_tileNdx]; }      
      Rect rect(int dx=0, int dy=0) const { return Rect(m_x*m_twidth+dx, m_y*m_theight+dy, m_twidth, m_theight); }

    private:
      Board::TileCollection& m_container;
      uint32_t m_x, m_y;
      uint32_t m_twidth, m_theight;
      uint32_t m_tileNdx;
    };

  public:
    TileRange(Board::TileCollection& c, uint32_t tw=0, uint32_t th=0) : c(c), tw(tw), th(th) {}
    TileRange& operator =(const TileRange& tr) { return *this; }
    Iterator begin() { return Iterator(0,0,0,c,tw,th); }
    Iterator end() { return Iterator(0,0,c.size(),c,tw,th); }

    Board::TileCollection& c;
    uint32_t tw, th;
  };
#pragma warning(default:4512)


  Board::Board(std::shared_ptr<Engine> engine, uint32_t dimension, uint32_t twidth, uint32_t theight)
      : m_engine(engine), m_dim(dimension), m_tileWidth(twidth), m_tileHeight(theight)
  {
    if ( dimension <= 2 )
      throw std::exception("Invalid board dimension, has to be >2");
    m_borderHoriz = (m_engine->GetWidth() - ( m_tileWidth * m_dim )) / 2;
    m_borderVert = (m_engine->GetHeight() - ( m_tileHeight * m_dim ) ) / 2;
    m_tiles.resize( dimension * dimension );
    //std::default_random_engine  generator;
    //m_diceColor = std::bind( std::uniform_int_distribution<uint32_t>(0,3), generator );
    //m_diceBool = std::bind( std::bernoulli_distribution(0.5), generator );
    m_label = std::unique_ptr<Text>(new Text(m_engine->GetFont("data/OpenSans-Bold.ttf",24), "hey", Colors::YELLOW));
  }
  
  void Board::Update(std::shared_ptr<Player> player)
  {
    auto elapsed = m_engine->GetTimerDelta();
    const auto& zeroms = std::chrono::milliseconds::zero();

    for (auto it : TileRange(m_tiles))
    {
      auto t = it.tile();      
    }
  }

  void Board::Draw()
  {
    DrawGrid();

    for (auto it : TileRange(m_tiles,m_tileWidth,m_tileHeight))
    {
      auto t = it.tile();
      auto r = it.rect(m_borderHoriz, m_borderVert);
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

  std::shared_ptr<Tile> Board::GetTile(uint32_t x, uint32_t y)
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