#include <gametimer.h>

namespace PuzzleGame
{

GameTimer::GameTimer(std::shared_ptr<Common> comm)
  : m_common(comm)
{
  m_lastTicks = m_common->GetTimerTicks();
}

void GameTimer::AddCallback( std::chrono::milliseconds period, std::function<void ()> func)
{
  Callback cb;
  cb.m_accumulated = std::chrono::milliseconds(0);
  cb.m_period = period;
  cb.m_periodFunc = func;
  m_callbacks.push_back(cb);
}

void GameTimer::Reset()
{
  m_callbacks.clear();
}

void GameTimer::Update()
{
  auto curTicks = m_common->GetTimerTicks();
  auto elapsedTicks = curTicks - m_lastTicks;
  m_lastTicks = curTicks;
  
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