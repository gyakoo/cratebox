#pragma once
#include <common.h>

namespace PuzzleGame
{
  class GameTimer
  {
  public:
    GameTimer(std::shared_ptr<Common> comm, std::chrono::milliseconds period);

    void SetPeriod(std::chrono::milliseconds period);
    bool Update();

  private:
    uint32_t m_lastTicks;
    std::shared_ptr<Common> m_common;
    std::chrono::milliseconds m_period;
    std::chrono::milliseconds m_accumulated;
  };

};