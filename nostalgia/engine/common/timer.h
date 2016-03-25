#ifndef _GYTIMER_H_
#define _GYTIMER_H_

class gyEngine;

class gyTimer : public gyReflectedObject
{
  GY_PIMPL_CLASS();
  GY_DECLARE_REFLECT(gyTimer);
public:
  gy_override int Create(gyVarDict& createParams);
  gy_override void FillCreateTemplate(gyVarDict& outTemplate);

  void Update();
  float GetAbsoluteTime();
  float GetFrameTime();

private:
  friend class gyEngine;
  gyTimer();
  ~gyTimer();
};

#endif