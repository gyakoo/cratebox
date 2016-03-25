#include <stdafx.h>
#include <engine/app/app.h>
#include <engine/gfx/renderer.h>

#include GY_PLATINC_APP

GY_REGISTER_REFLECT(gyDefaultApp);

gyAppWindowMessageData::gyAppWindowMessageData()
  : gyCallbackData(&gyApp::OnWindowMessage)
{
  for ( int i = 0; i < 4; ++i ) 
    params[i] = 0;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
gyCallback gyApp::OnWindowMessage;
gyApp::gyApp()
{
  impl = new gyApp::Impl;
}

gyApp::~gyApp()
{
  RV_FAIL_IF( !impl, "Invalid app implementation" );
  SafeDestroyDelete(impl);
}

int gyApp::Create(gyVarDict& createParams)
{
  R_FAIL_IF( !impl, "Invalid app implementation" );
  if ( impl->Create(createParams) != R_OK )
    return -1;  

  // init renderer before was here
  return R_OK;
}

int gyApp::Run( )
{
  R_FAIL_IF( !impl, "Invalid app implementation" );
  int res = impl->Run(*this);  
  return res;
}

void gyApp::SetWindowTitle(const char* title)
{
  RV_FAIL_IF( !impl, "Invalid app implementation" );
  impl->SetWindowTitle(title);  
}

void gyApp::Terminate()
{
  impl->Terminate();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
gyDefaultApp::gyDefaultApp()
  : fpstime(0)
{
  *fpstext = 0;
}

int gyDefaultApp::OnRun()
{
  /* player->update() (camera update)
   * Visibility update
   * Game update
   * 
  */ 


  gyGetRenderer().Render();

  fpstime += gyGetTimer().GetFrameTime();
  if (fpstime>1.0)
  {
    fpstime-=1.0;
    sprintf_s(fpstext,"%.0lf fps",1.0/fpstime);
    SetWindowTitle(fpstext);
  }

  gyGetSoundManager().Update();
  return R_OK;
}