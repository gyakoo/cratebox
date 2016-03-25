#ifndef _APP_H_
#define _APP_H_

#include <engine/gfx/renderer.h>
#include <engine/common/callback.h>

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
struct gyAppWindowMessageData : public gyCallbackData
{
  gyAppWindowMessageData();
  uint64_t params[4];
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class gyApp : public gyReflectedObject
{
  GY_PIMPL_CLASS();
public:
  gyApp();
  ~gyApp();
  gy_override int Create( gyVarDict& createParams );
  int Run();
  void Terminate();  
  void SetWindowTitle(const char* title);
  static gyCallback OnWindowMessage;

protected:
  virtual int OnRun() = 0;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class gyDefaultApp : public gyApp
{
  GY_DECLARE_REFLECT(gyDefaultApp);
public:
  gyDefaultApp();

protected:
  gy_override int OnRun();

  char fpstext[32];
  double fpstime;
};

#endif