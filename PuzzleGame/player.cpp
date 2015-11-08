#include <player.h>

namespace PuzzleGame
{
  Player::Player(std::shared_ptr<Common> comm, std::shared_ptr<Board> board)
    : m_common(comm), m_board(board)
  {
    m_common->AddKeyListener(this);
    m_left = m_top = m_board->GetDimension()/2 - 1;
    m_logoTiles[0] = LTT_R;
    m_logoTiles[1] = LTT_G;
    m_logoTiles[2] = LTT_Y;
    m_logoTiles[3] = LTT_B;
    m_board->SetTimeUntilNext(1);
    m_board->OnPlayerAdvanced( m_left, m_top );
  }

  Player::~Player()
  {
    m_common->RemoveKeyListener(this);
  }

  void Player::OnKeyDown(int scancode)
  {
    switch (scancode)
    {
    case SDL_SCANCODE_LEFT : Move(-1,0); break;
    case SDL_SCANCODE_RIGHT: Move(+1,0); break;
    case SDL_SCANCODE_UP   : Move(0,-1); break;
    case SDL_SCANCODE_DOWN : Move(0,+1); break;
    case SDL_SCANCODE_Z: Rotate<true>(); break;
    case SDL_SCANCODE_X: Rotate<false>(); break;
    }
  }

  void Player::Update()
  {
  }

  void Player::Draw()
  {
    std::array<Rect,4> rects = {
            m_board->GetTileRect(m_left,m_top),
            m_board->GetTileRect(m_left+1,m_top),
            m_board->GetTileRect(m_left+1,m_top+1),
            m_board->GetTileRect(m_left,m_top+1) };

    Color color;
    for ( int i =0; i < 4; ++i )
    {
      switch ( m_logoTiles[i] )
      {
      case LTT_R: color = Colors::MSRED; break;
      case LTT_G: color = Colors::MSGREEN; break;
      case LTT_B: color = Colors::MSBLUE; break;
      case LTT_Y: color = Colors::MSYELLOW; break;
      default: color.a = 0;
      }
      if ( color.a )
        m_common->FillRect( rects[i], color );
    }
  }

  void Player::Move(int dx, int dy)
  {
    const int nx=m_left + dx;
    const int ny=m_top + dy;
    if ( CanMoveTo(nx,ny) )
    {
      m_left = nx;
      m_top = ny;
      m_board->OnPlayerAdvanced( m_left, m_top );
    }
  }

  bool Player::CanMoveTo(int left, int top)
  {
    // out of board?
    const int dim = (int)m_board->GetDimension();
    if ( left < 0 || left >= dim ||
      top < 0 || top >= dim )
      return false;

    // there's a mismatch piece in the way
    const uint32_t l=(uint32_t)left;
    const uint32_t t=(uint32_t)top;
    std::array<Board::Tile,4> dstTiles = { 
      m_board->GetTile(l,t), m_board->GetTile(l+1,t),
      m_board->GetTile(l+1,t+1), m_board->GetTile(l,t+1) };
    for (auto i = 0; i < dstTiles.size(); ++i )
    {
      auto& t = dstTiles[i];
      if ( t.m_type == Board::TT_PIECE && t.m_data != (uint32_t)m_logoTiles[i] ) 
        return false;
    }
    return true;
  }

  template<bool LEFT>
  void Player::Rotate()
  {
    if ( LEFT )
      std::rotate( m_logoTiles.begin(), m_logoTiles.begin()+1, m_logoTiles.end() );
    else
      std::rotate( m_logoTiles.rbegin(), m_logoTiles.rbegin()+1, m_logoTiles.rend() );
    m_board->OnPlayerAdvanced( m_left, m_top );
  }
};