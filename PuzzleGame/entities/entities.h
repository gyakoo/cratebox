#pragma once
#include <base/common.h>
#include <base/gametimer.h>
#include <base/fsm.h>

namespace Game
{
  class Entity
  {
  public:
    Entity(uint32_t x, uint32_t y, std::chrono::milliseconds updatePeriod)
      : m_x(x), m_y(y), m_updatePeriod(updatePeriod), m_timeToUpdate(std::chrono::milliseconds::zero())
      , m_markForRemove(false)
    {}

    void Update(std::chrono::milliseconds dt);
    void Draw(Engine& engine);

    bool IsMarkedForRemove(){ return m_markForRemove; }
    
  protected:
    virtual void OnUpdate(std::chrono::milliseconds ){}
    virtual void OnDraw(Engine&){}

    uint32_t m_x, m_y;
    std::chrono::milliseconds m_timeToUpdate;
    std::chrono::milliseconds m_updatePeriod;
    bool m_markForRemove;
  };

};