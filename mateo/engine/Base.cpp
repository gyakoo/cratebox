#include <Pch.h>
#include "Base.h"
#include <engine/TestRenderPipeline.h>
#include <engine/DxDevice.h>
#include <engine/DxHelper.h>
#include <engine/RenderPipeline.h>

using namespace Platform;
using namespace concurrency;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;

namespace Engine
{

  Base::Base(CoreApplicationView^ appView, bool setEventHandlers)
  {
    if (setEventHandlers)
    {
      // lifecycle handlers
      appView->Activated += ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &Base::OnActivated);
      CoreApplication::Suspending += ref new EventHandler<SuspendingEventArgs^>(this, &Base::OnSuspending);
      CoreApplication::Resuming += ref new EventHandler<Platform::Object^>(this, &Base::OnResuming);
    }

    m_dxDevice = std::make_unique<DxDevice>(this);

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */

  }

  void Base::OnAppLoad(Platform::String^ entryPoint)
  {

  }

  void Base::Update()
  {
    m_timer.Tick([&]()
    {
      // TODO: Replace this with your app's content update functions.      
      for (auto rpip : m_renderPipelines)
      {
        rpip->Update(m_timer);
      }
    });

  }

  void Base::Render()
  {
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
      return ;

    for (auto rpip : m_renderPipelines)
    {
      rpip->Render();
    }

    // Render the scene objects.
    m_dxDevice->Present();
  }

  void Base::SetWindow(CoreWindow^ window, bool setEventHandlers)
  {
    m_window = window;

    if (setEventHandlers)
    {
      // window handlers
      window->SizeChanged += ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &Base::OnWindowSizeChanged);
      window->VisibilityChanged += ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &Base::OnVisibilityChanged);
      window->Closed += ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &Base::OnWindowClosed);

      // display info handlers
      DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();
      currentDisplayInformation->DpiChanged += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &Base::OnDpiChanged);
      currentDisplayInformation->OrientationChanged += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &Base::OnOrientationChanged);
      DisplayInformation::DisplayContentsInvalidated += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &Base::OnDisplayContentsInvalidated);
    }

    m_dxDevice->SetWindow(window);

    for (auto& rpip : m_renderPipelines)
    {
        rpip->CreateResources();
        rpip->ReloadWindowSizeResources();
    }
  }


  // Application lifecycle event handlers.
  void Base::OnActivated(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView, Windows::ApplicationModel::Activation::IActivatedEventArgs^ args)
  {

  }

  void Base::OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ args)
  {
    // Save app state asynchronously after requesting a deferral. Holding a deferral
    // indicates that the application is busy performing suspending operations. Be
    // aware that a deferral may not be held indefinitely. After about five seconds,
    // the app will be forced to exit.
    SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();

    create_task([this, deferral]()
    {
      m_dxDevice->Trim();
      // Insert your code here.
      deferral->Complete();
    });
  }

  void Base::OnResuming(Platform::Object^ sender, Platform::Object^ args)
  {

  }

  // Window event handlers.
  void Base::OnWindowSizeChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ args)
  {
    m_dxDevice->SetLogicalSize(Size(sender->Bounds.Width, sender->Bounds.Height));
  }

  void Base::OnVisibilityChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ args)
  {

  }

  void Base::OnWindowClosed(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::CoreWindowEventArgs^ args)
  {

  }

  // DisplayInformation event handlers.
  void Base::OnDpiChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args)
  {
    // Note: The value for LogicalDpi retrieved here may not match the effective DPI of the app
    // if it is being scaled for high resolution devices. Once the DPI is set on DeviceResources,
    // you should always retrieve it using the GetDpi method.
    // See DeviceResources.cpp for more details.
    m_dxDevice->SetDpi(sender->LogicalDpi);
  }

  void Base::OnOrientationChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args)
  {
    m_dxDevice->SetCurrentOrientation(sender->CurrentOrientation);
  }

  void Base::OnDisplayContentsInvalidated(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args)
  {
    m_dxDevice->ValidateDevice();
  }

  void Base::DeviceLost()
  {
      for (auto rpip : m_renderPipelines)
      {
          rpip->ReleaseResources();
      }
  }

  void Base::DeviceRestored()
  {
      for (auto rpip : m_renderPipelines)
      {
          rpip->CreateResources();
      }
  }

  void Base::ReloadWindowSizeResources()
  {
      for (auto rpip : m_renderPipelines)
      {
          rpip->ReloadWindowSizeResources();
      }
  }

  int Base::RegisterRenderPipeline(Platform::String^ pipelineName)
  {
      int ret = -1;
      if (pipelineName == "TestRenderPipeline")
      {
          ret = (int)m_renderPipelines.size();
          auto rpip = std::make_shared<TestRenderPipeline>();
          m_renderPipelines.push_back(rpip);          
      }

      ThrowIfAssert(ret != -1, L"No render pipeline created");
      return ret;
  }

  void Base::UnregisterRenderPipeline(int cbIndexAndOffset)
  {
      if (cbIndexAndOffset >= 0 && cbIndexAndOffset < (int)m_renderPipelines.size())
      {
          m_renderPipelines.erase(m_renderPipelines.begin() + cbIndexAndOffset);
      }
  }

}; // namespace