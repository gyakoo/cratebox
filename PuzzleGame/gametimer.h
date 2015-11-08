#pragma once
#include <common.h>

namespace PuzzleGame
{
  class GameTimer
  {
  public:
    GameTimer(std::shared_ptr<Common> comm);

    uint32_t AddCallback( std::chrono::milliseconds period, std::function<void ()> func);
    void RemoveCallback(uint32_t cbIndex);
    void Update();
    void Reset();

  private:
    struct Callback
    {
      std::chrono::milliseconds m_period;
      std::chrono::milliseconds m_accumulated;
      std::function<void (void)> m_periodFunc;
    };

    uint32_t m_lastTicks;
    std::shared_ptr<Common> m_common;
    std::vector<Callback> m_callbacks;
  };

};