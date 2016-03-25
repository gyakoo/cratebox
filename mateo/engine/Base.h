#pragma once
#include <engine/StepTimer.h>

namespace Engine
{
    class DxDevice;
    class RenderPipeline;

    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Base sealed
    {
    public:
      Base(Windows::ApplicationModel::Core::CoreApplicationView^ appView, bool setEventHandlers);

      void Update();
      void Render();
      void SetWindow(Windows::UI::Core::CoreWindow^ window, bool setEventHandlers);
      void OnAppLoad(Platform::String^ entryPoint);
      void DeviceLost();
      void DeviceRestored();
      void ReloadWindowSizeResources();

      int RegisterRenderPipeline(Platform::String^ pipelineName);
      void UnregisterRenderPipeline(int cbIndexAndOffset);

    private:
      // Application lifecycle event handlers.
      void OnActivated(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView, Windows::ApplicationModel::Activation::IActivatedEventArgs^ args);
      void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ args);
      void OnResuming(Platform::Object^ sender, Platform::Object^ args);

      // Window event handlers.
      void OnWindowSizeChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ args);
      void OnVisibilityChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ args);
      void OnWindowClosed(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::CoreWindowEventArgs^ args);

      // DisplayInformation event handlers.
      void OnDpiChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
      void OnOrientationChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
      void OnDisplayContentsInvalidated(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);

    private:
      Platform::Agile<Windows::UI::Core::CoreWindow> m_window;
      std::unique_ptr<DxDevice> m_dxDevice;
      StepTimer m_timer;
      std::vector< std::shared_ptr<RenderPipeline> > m_renderPipelines;
    };
}
