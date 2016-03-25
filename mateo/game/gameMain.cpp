#include <Pch.h>
#include <game/GameMain.h>

using namespace Game;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;

GameMain::GameMain(Windows::ApplicationModel::Core::CoreApplicationView^ appView)
{
  m_engineBase = ref new Engine::Base(appView, true);

  // test
  m_engineBase->RegisterRenderPipeline("TestRenderPipeline");
}

GameMain::~GameMain()
{
}

void GameMain::SetWindow(Windows::UI::Core::CoreWindow^ window)
{
  m_engineBase->SetWindow(window, true);
}

bool GameMain::Run()
{
  m_engineBase->Update();
  m_engineBase->Render();
  return true;
}

void GameMain::Load(Platform::String^ entryPoint)
{
  m_engineBase->OnAppLoad(entryPoint);
}