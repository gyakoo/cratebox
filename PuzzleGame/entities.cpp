#include <entities.h>
#include <player.h>

namespace PuzzleGame
{

  //void Entity::Update(std::chrono::milliseconds dt, Board& board, Player& player)
  //{
  //  m_timeToUpdate -= dt;
  //  if ( m_timeToUpdate <= std::chrono::milliseconds::zero() )
  //  {
  //    m_timeToUpdate = m_updatePeriod + m_timeToUpdate;
  //    OnUpdate(dt, board, player);
  //  }
  //}

  //void Entity::Draw(Engine& engine, Board& board)
  //{
  //  OnDraw(engine, board);
  //}

  //Chaser::Chaser(Board& board, uint32_t x, uint32_t y, Player::LogoTileType target)
  //  : Entity(x,y,std::chrono::milliseconds(1000)), m_target(target)
  //{
  //  board.GetTile(m_x, m_y).SetBlocked();
  //}

  //void Chaser::OnUpdate(std::chrono::milliseconds dt, Board& board, Player& player)
  //{
  //  uint32_t tx=0, ty=0;
  //  player.GetPosOf( m_target, tx, ty);
  //  
  //  int dx = tx-m_x; 
  //  int dy = ty-m_y; 
  //  int absdx = std::abs(dx);
  //  int absdy = std::abs(dy);
  //  int dxnormal = absdx ? (int)(dx/absdx) : 0;
  //  int dynormal = absdy ? (int)(dy/absdy) : 0;
  //  int newx = m_x;
  //  int newy = m_y;
  //  if ( !dynormal || (dxnormal && dxnormal < dynormal)  )
  //  {
  //    newx += dxnormal;
  //  }
  //  else
  //  {
  //    newy += dynormal;
  //  }
  //  board.GetTile(m_x, m_y).SetNone();
  //  auto& newTile = board.GetTile(newx, newy);
  //  if ( !newTile.IsNone() )
  //  {
  //    // collision with anything in board (except the player)
  //    m_markForRemove = true;
  //  }
  //  else if ( player.IsPiece(m_target,newx,newy) )
  //  {
  //    // collision with target piece
  //    m_markForRemove = true;
  //    player.BrokenPiece(m_target);
  //  }

  //  m_x = newx;
  //  m_y = newy;
  //  if ( !m_markForRemove )
  //    newTile.SetBlocked();
  //}

  //void Chaser::OnDraw(Engine& engine, Board& board )
  //{
  //  if ( m_markForRemove ) return;
  //  auto r = board.GetTileRect(m_x, m_y);
  //  r.Translate(1,1);
  //  r.Deflate(43,43);
  //  static std::array<Color,4> s_colors={
  //    Colors::MSRED, Colors::MSGREEN, 
  //    Colors::MSYELLOW, Colors::MSBLUE };
  //  engine.FillRect(r, s_colors[m_target]);
  //}

 };