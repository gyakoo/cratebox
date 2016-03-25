#include <stdafx.h>
#include <engine/common/timer.h>

#include GY_PLATINC_TIMER

gyTimer::gyTimer()
{
  impl = new Impl;
}

int gyTimer::Create(gyVarDict& createParams)
{
  return R_OK;
}

void gyTimer::FillCreateTemplate(gyVarDict& outTemplate)
{
}

gyTimer::~gyTimer()
{
  SafeDestroyDelete(impl);
}
 
float gyTimer::GetAbsoluteTime()
{
  return impl->GetAbsoluteTime();
}

float gyTimer::GetFrameTime()
{
  return impl->GetFrameTime();
}

void gyTimer::Update()
{
  impl->Update();
}
