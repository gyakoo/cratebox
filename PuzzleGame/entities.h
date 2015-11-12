#pragma once
#include <common.h>
#include <gametimer.h>
#include <board.h>
#include <player.h>
#include <fsm.h>

namespace PuzzleGame
{
  //class Entity
  //{
  //public:
  //  Entity(uint32_t x, uint32_t y, std::chrono::milliseconds updatePeriod)
  //    : m_x(x), m_y(y), m_updatePeriod(updatePeriod), m_timeToUpdate(std::chrono::milliseconds::zero())
  //    , m_markForRemove(false)
  //  {}

  //  void Update(std::chrono::milliseconds dt, Board& board, Player& player);
  //  void Draw(Engine& engine, Board& board);

  //  bool IsMarkedForRemove(){ return m_markForRemove; }
  //  
  //protected:
  //  virtual void OnUpdate(std::chrono::milliseconds , Board& , Player& ){}
  //  virtual void OnDraw(Engine& , Board& ){}

  //  uint32_t m_x, m_y;
  //  std::chrono::milliseconds m_timeToUpdate;
  //  std::chrono::milliseconds m_updatePeriod;
  //  bool m_markForRemove;
  //};

  //class Chaser : public Entity
  //{
  //public:
  //  Chaser(Board& board, uint32_t x, uint32_t y, Player::LogoTileType target);

  //protected:
  //  virtual void OnUpdate(std::chrono::milliseconds dt, Board& board, Player& player);
  //  virtual void OnDraw(Engine& engine, Board& board);
  //  Player::LogoTileType m_target;
  //};

};