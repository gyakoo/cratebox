#include <stdafx.h>
#include <engine/input/input.h>

gyInputTriggerOpts gyInputTriggerOpts::DEFAULT;

#include GY_PLATINC_INPUT

gyInputTrigger::gyInputTrigger()
  : sibling(NULL), hash(0)
{
}

gyInputTrigger::~gyInputTrigger()
{
  SafeRelease(sibling);
}



////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
gyInputPadVibrationFade::gyInputPadVibrationFade(float targetL, float targetR, float duration)
  : tgt(targetL,targetR), timeInSec(duration), curTime(duration), src(.0f)
{
  
}

void gyInputPadVibrationFade::Invalidate()
{
  timeInSec = curTime = -1.0f;
}

void gyInputPadVibrationFade::Update(float dt, gyv2f& outRes)
{
  curTime -= dt;
  if ( curTime > 0.0f )
  {
    const float timeFrac = 1.0f - curTime/timeInSec;
    outRes.x = src.x + (tgt.x-src.x)*timeFrac;
    outRes.y = src.y + (tgt.y-src.y)*timeFrac;
  }
  else
  {
    outRes = tgt;
    Invalidate();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
gyInput::gyInput()
{
  impl = new Impl;
  impl->Init();
}

gyInput::~gyInput()
{
  triggers.ReleaseAll();
  
  if ( impl )
    impl->Destroy();
  SafeDelete(impl);
}

void gyInput::Recycle()
{
  impl->Recycle();
}

int gyInput::Create(gyVarDict& createParams)
{
  return R_OK;
}

void gyInput::FillCreateTemplate(gyVarDict& outTemplate)
{
  
}

struct SameTriggerHashFunc
{
  uint32_t h;

  SameTriggerHashFunc(gyInputTrigger* trigg)
  {
    h = trigg->ComputeHash();
  }

  bool operator()( const gyResourceMgr<gyInputTrigger,ID_INPUTTRIGGER>::ResourceInfo& rinfo )
  {
    gyInputTrigger* trigg = rinfo.resource;
    while ( trigg )
    {
      if ( trigg->hash == h )
        return true;
      trigg = trigg->sibling;
    }
    return false;
  }

};

gyIDInputTrigger gyInput::TriggerCreate( gyInputDevice devType, gyInputIndex iindex, const gyInputTriggerOpts& opts )
{
  gySharedPtr<gyInputTrigger> trigger = impl->TriggerCreate(devType, iindex, opts);  
  if ( !trigger )
    return gyIDInputTrigger::INVALID();

  return triggers.AddUnique(trigger, SameTriggerHashFunc(trigger));
}

int gyInput::TriggerCreate( gyIDInputTrigger existId, gyInputDevice devType, gyInputIndex iindex, const gyInputTriggerOpts& opts )
{
  R_FAIL_IF(!existId.IsValid(), "Passed id is invalid" );

  gyInputTrigger* trigger = triggers.Get(existId);
  R_FAIL_IF( !trigger, "Valid Id but NO trigger is found" );

  gySharedPtr<gyInputTrigger> newTrigger = impl->TriggerCreate(devType, iindex, opts);  
  R_FAIL_IF( !newTrigger, "Can't create new trigger" );

  // put at the end of list
  while ( trigger->sibling )
    trigger = trigger->sibling;

  newTrigger->AddRef();
  trigger->sibling = newTrigger;  
  return R_OK;
}


void gyInput::TriggerRelease(gyIDInputTrigger& triggerId)
{
  if ( !triggerId.IsValid() )
    return;
  triggers.Release(triggerId);
}

float gyInput::TriggerGet(gyIDInputTrigger triggerId)
{
  if ( !triggerId.IsValid() )
    return 0.0f;
  
  gyInputTrigger* trigger = triggers.Get(triggerId);
  float maxValue = 0.0f;
  float cur = 0.0f;
  while ( trigger )
  {
    cur = trigger->Get();
    maxValue = gyMath::Abs(maxValue) > gyMath::Abs(cur) ? maxValue : cur;
    trigger = trigger->sibling;
  }
  return maxValue;
}

bool gyInput::CheckCompatibilityDeviceIndex(gyInputDevice devType, gyInputIndex iindex )
{
  switch ( devType )
  {
  case GY_INPUTDEV_KEYBOARD : return iindex>=GYII_KB_FIRST_ELEMENT && iindex <=GYII_KB_LAST_ELEMENT;
  case GY_INPUTDEV_MOUSE    : return iindex>=GYII_MO_FIRST_ELEMENT && iindex <= GYII_MO_LAST_ELEMENT;
  case GY_INPUTDEV_GAMEPAD_0:
  case GY_INPUTDEV_GAMEPAD_1:
  case GY_INPUTDEV_GAMEPAD_2:
  case GY_INPUTDEV_GAMEPAD_3: return iindex>=GYII_PAD_FIRST_ELEMENT && iindex <= GYII_PAD_LAST_ELEMENT;
  }
  return false;
}


void gyInput::SetPadDeadZone(float deadZone){ impl->SetPadDeadZone(deadZone); }
void gyInput::SetPadDeadZoneEnabled(bool status){ impl->SetPadDeadZoneEnabled(status); }
void gyInput::SetPadNormalizeAnalogEnabled(bool status){ impl->SetPadNormalizeAnalogEnabled(status); }
bool gyInput::IsPadDeadZoneEnabled(){ return impl->IsPadDeadZoneEnabled(); }
bool gyInput::IsPadNormalizeAnalogEnabled(){ return impl->IsPadNormalizeAnalogEnabled(); }
float gyInput::GetPadDeadZone(){ return impl->GetPadDeadZone(); }
bool gyInput::IsDeviceConnected(gyInputDevice dev){ return impl->IsDeviceConnected(dev); }
int gyInput::GetPadVibrationSupport(gyInputDevice pad){ return impl->GetPadVibrationSupport(pad) ? R_OK : -1; }
int gyInput::SetPadVibration(gyInputDevice pad, float left, float right, const gyInputPadVibrationFade* fade){ return impl->SetPadVibration(pad,left,right,fade); }