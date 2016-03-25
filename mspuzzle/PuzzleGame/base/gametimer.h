#pragma once
#include <base/common.h>

namespace Game
{
  class GameTimer
  {
  public:
    GameTimer(std::shared_ptr<Engine> engine);

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

    std::shared_ptr<Engine> m_engine;
    std::vector<Callback> m_callbacks;
  };

};