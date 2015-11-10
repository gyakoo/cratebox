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
      {
        rects[i].Translate(1,1);
        rects[i].Deflate(7,7);
        m_common->FillRect( rects[i], color );
        rects[i].Deflate(30,30);
        m_common->FillRect( rects[i], Colors::GRAYWORKSPACE );
      }
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
      MatchPieces();
    }
  }

  void Player::MatchPieces()
  {
    std::array<Board::Tile,4> dstTiles;
    GetOverlappingPieces(m_left, m_top, dstTiles);
    std::array<std::pair<uint32_t,uint32_t>,4> positions={
      std::make_pair(m_left,m_top), std::make_pair(m_left+1,m_top),
      std::make_pair(m_left+1,m_top+1), std::make_pair(m_left,m_top+1)};
    for (auto i = 0u; i < dstTiles.size(); ++i )
    {
      auto& t = dstTiles[i];
      if ( t.m_type == Board::TT_PIECE && 
        t.m_data == (uint32_t)m_logoTiles[i] )
      {
        auto& pos = positions[i];
        m_board->MatchPiece(m_left, m_top, pos.first, pos.second);
      }
    }
  }

  void Player::GetOverlappingPieces(uint32_t left, uint32_t top, std::array<Board::Tile,4>& outPieces)
  {
    outPieces[0] = m_board->GetTile(left,top);
    outPieces[1] = m_board->GetTile(left+1,top);
    outPieces[2] = m_board->GetTile(left+1,top+1);
    outPieces[3] = m_board->GetTile(left,top+1);
  }

  bool Player::CanMoveTo(int left, int top)
  {
    // out of board?
    const int dim = (int)m_board->GetDimension();
    if ( left < 0 || left >= dim-1 ||
      top < 0 || top >= dim-1 )
      return false;

    // is there a mismatch piece in the way?
    std::array<Board::Tile,4> dstTiles;
    GetOverlappingPieces((uint32_t)left, (uint32_t)top, dstTiles);
    for (auto i = 0u; i < dstTiles.size(); ++i )
    {
      auto& t = dstTiles[i];
      if ( t.m_type == Board::TT_PIECE && 
        t.m_data != (uint32_t)m_logoTiles[i] ) 
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