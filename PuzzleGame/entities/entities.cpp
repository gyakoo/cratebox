#include <entities/entities.h>

namespace Game
{

  void Entity::Update(std::chrono::milliseconds dt)
  {
    m_timeToUpdate -= dt;
    if ( m_timeToUpdate <= std::chrono::milliseconds::zero() )
    {
      m_timeToUpdate = m_updatePeriod + m_timeToUpdate;
      OnUpdate(dt);
    }
  }

  void Entity::Draw(Engine& engine)
  {
    OnDraw(engine);
  }
 };