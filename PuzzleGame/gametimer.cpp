#include <gametimer.h>

namespace PuzzleGame
{

GameTimer::GameTimer(std::shared_ptr<Common> comm, std::chrono::milliseconds period)
  : m_common(comm), m_period(period), m_accumulated(0)
{
  m_lastTicks = m_common->GetTimerTicks();
}

void GameTimer::SetPeriod(std::chrono::milliseconds period)
{
  m_period = period;
  m_accumulated = std::chrono::milliseconds(0);
}

bool GameTimer::Update()
{
  auto curTicks = m_common->GetTimerTicks();
  auto elapsedTicks = curTicks - m_lastTicks;
  m_lastTicks = curTicks;
  m_accumulated += std::chrono::milliseconds(elapsedTicks);
  bool ret = false;
  if ( m_accumulated >= m_period )
  {
    ret = true;
    m_accumulated -= m_period;
  }
  return ret;
}

};