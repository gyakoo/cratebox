#include <gametimer.h>

namespace PuzzleGame
{

GameTimer::GameTimer(std::shared_ptr<Engine> engine)
  : m_engine(engine)
{
}

uint32_t GameTimer::AddCallback( std::chrono::milliseconds period, std::function<void ()> func)
{
  Callback cb;
  cb.m_accumulated = std::chrono::milliseconds(0);
  cb.m_period = period;
  cb.m_periodFunc = func;
  m_callbacks.push_back(cb);
  return m_callbacks.size()-1;
}

void GameTimer::RemoveCallback(uint32_t cbIndex)
{
  if ( cbIndex < m_callbacks.size() )
    m_callbacks.erase( m_callbacks.begin() + cbIndex );
}

void GameTimer::Reset()
{
  m_callbacks.clear();
}

void GameTimer::Update()
{
  auto elapsedTicks = m_engine->GetTimerDelta();
  
  std::for_each(m_callbacks.begin(), m_callbacks.end(), [elapsedTicks](Callback& cb)
  {
    cb.m_accumulated += std::chrono::milliseconds(elapsedTicks);
    if ( cb.m_accumulated >= cb.m_period )
    {
      cb.m_accumulated -= cb.m_period;
      if ( cb.m_periodFunc )
        cb.m_periodFunc();
    } 
  });
}

};