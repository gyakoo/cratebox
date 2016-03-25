#ifndef _GYINPUT_H_
#define _GYINPUT_H_

/* GOALS
 * dead zones
 * analog signal filtering
 * button sequences (chords)
 * gesture
 * multiple players
 * multiplatform
 * controller remapping
 * context sensitive inputs
 * disabling of certain inputs
*/

#include <engine/input/inputtypes.h>

class gyEngine;
class gyInputPadVibrationFade;

//////////////////////////////////////////////////////////////////////////
struct gyInputTriggerOpts
{
  gyInputTriggerOpts(bool once=false, bool oncePerFrame=false, float deadZone=.0f)
    : once(once), oncePerFrame(oncePerFrame), deadZone(deadZone)
  {
  }

  static gyInputTriggerOpts DEFAULT;
  float deadZone;
  bool once;
  bool oncePerFrame;
};

//////////////////////////////////////////////////////////////////////////
class gyInputTrigger : public gyResourceBase
{
public:
  gyInputTrigger();
  ~gyInputTrigger();
  virtual float Get() = 0;
  gy_override uint32_t ComputeHash(){ return hash; }

  gyInputTriggerOpts opts;
  gyInputTrigger* sibling;
  uint32_t hash;
};

//////////////////////////////////////////////////////////////////////////
class gyInput : public gyReflectedObject
{
  GY_PIMPL_CLASS();
  GY_DECLARE_REFLECT(gyInput);
public:
  gy_override int Create(gyVarDict& createParams);
  gy_override void FillCreateTemplate(gyVarDict& outTemplate);
  void Recycle();

  gyIDInputTrigger TriggerCreate( gyInputDevice devType, gyInputIndex iindex, const gyInputTriggerOpts& opts=gyInputTriggerOpts::DEFAULT );
  int TriggerCreate( gyIDInputTrigger existId, gyInputDevice devType, gyInputIndex iindex, const gyInputTriggerOpts& opts=gyInputTriggerOpts::DEFAULT );
  float TriggerGet(gyIDInputTrigger triggerId);
  void TriggerRelease(gyIDInputTrigger& triggerId);
  bool IsDeviceConnected(gyInputDevice dev);
  bool CheckCompatibilityDeviceIndex(gyInputDevice devType, gyInputIndex iindex );

  // gamepad specific
  void SetPadDeadZone(float deadZone);
  void SetPadDeadZoneEnabled(bool status);
  void SetPadNormalizeAnalogEnabled(bool status);
  bool IsPadDeadZoneEnabled();
  bool IsPadNormalizeAnalogEnabled();
  float GetPadDeadZone();
  int GetPadVibrationSupport(gyInputDevice pad);
  int SetPadVibration(gyInputDevice pad, float left, float right, const gyInputPadVibrationFade* fade=0);

private:
  friend class gyEngine;
  gyInput();
  ~gyInput();

  gyResourceMgr<gyInputTrigger, ID_INPUTTRIGGER> triggers;
};

//////////////////////////////////////////////////////////////////////////
class gyInputPadVibrationFade
{
public:
  gyInputPadVibrationFade(float targetL=.0f, float targetR=.0f, float duration=-1.0f);

private:
  friend class gyInput::Impl;
  void Invalidate();
  void Update(float dt, gyv2f& outRes);

  gyv2f src;
  gyv2f tgt;
  float curTime;
  float timeInSec;
};
#endif