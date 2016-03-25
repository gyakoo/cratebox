#include <engine/common/clock.h>

//////////////////////////////////////////////////////////////////////////
// gyTimerImpl INTERFACE
//////////////////////////////////////////////////////////////////////////
class gyTimer::Impl
{
public:
  Impl();
  void Destroy();
  float GetFrameTime();
  float GetAbsoluteTime();
  void Update();

  gyClock mainClock;
  double frameTime;
  double absTime;
};


//////////////////////////////////////////////////////////////////////////
// gyTimerImpl IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////
gyTimer::Impl::Impl() : frameTime(1.0/60.0), absTime(0.0)
{
  mainClock.Start();
}

void gyTimer::Impl::Destroy()
{
  mainClock.Stop();
}

float gyTimer::Impl::GetFrameTime()
{
  return (float)frameTime;
}

float gyTimer::Impl::GetAbsoluteTime()
{
  return (float)absTime;
}

void gyTimer::Impl::Update()
{
  frameTime = mainClock.Split();
  absTime += frameTime;
}