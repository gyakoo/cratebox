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
    m_left += dx;
    m_top += dy;
  }

  template<bool LEFT>
  void Player::Rotate()
  {
    if ( LEFT )
      std::rotate( m_logoTiles.begin(), m_logoTiles.begin()+1, m_logoTiles.end() );
    else
      std::rotate( m_logoTiles.rbegin(), m_logoTiles.rbegin()+1, m_logoTiles.rend() );
  }
};