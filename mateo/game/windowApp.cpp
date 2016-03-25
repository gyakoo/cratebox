#include <Pch.h>
#include <game/WindowApp.h>

#include <ppltasks.h>

using namespace Game;

using namespace concurrency;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;

// The main function is only used to initialize our IFrameworkView class.
[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
	auto appSourceCreator = ref new WindowAppSource();
	CoreApplication::Run(appSourceCreator);
	return 0;
}

IFrameworkView^ WindowAppSource::CreateView()
{
	return ref new WindowApp();
}

WindowApp::WindowApp() :
	m_windowClosed(false),
	m_windowVisible(true)
{
}

// The first method called when the IFrameworkView is being created.
void WindowApp::Initialize(CoreApplicationView^ applicationView)
{
	// Register event handlers for app lifecycle. 
	applicationView->Activated += ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &WindowApp::OnActivated);
	CoreApplication::Suspending += ref new EventHandler<SuspendingEventArgs^>(this, &WindowApp::OnSuspending);
  CoreApplication::Resuming += ref new EventHandler<Platform::Object^>(this, &WindowApp::OnResuming);

	// At this point we have access to the device. We can create the device-dependent resources.  
  m_GameMain = std::make_unique<GameMain>(applicationView);
}

// Called when the CoreWindow object is created (or re-created).
void WindowApp::SetWindow(CoreWindow^ window)
{
	window->SizeChanged += ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &WindowApp::OnWindowSizeChanged);
	window->VisibilityChanged += ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &WindowApp::OnVisibilityChanged);
	window->Closed += ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &WindowApp::OnWindowClosed);

	DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();
	currentDisplayInformation->DpiChanged += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &WindowApp::OnDpiChanged);
	currentDisplayInformation->OrientationChanged += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &WindowApp::OnOrientationChanged);
	DisplayInformation::DisplayContentsInvalidated += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &WindowApp::OnDisplayContentsInvalidated);

  m_GameMain->SetWindow(window);
}

// Initializes scene resources, or loads a previously saved app state.
void WindowApp::Load(Platform::String^ entryPoint)
{
  m_GameMain->Load(entryPoint);
}

// This method is called after the window becomes active.
void WindowApp::Run()
{
	while (!m_windowClosed)
	{
		if (m_windowVisible)
		{
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
            m_GameMain->Run();
		}
		else
		{
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
		}
	}
}

void WindowApp::Uninitialize()
{
}

void WindowApp::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
{
	// Run() won't start until the CoreWindow is activated.
	CoreWindow::GetForCurrentThread()->Activate();
}

void WindowApp::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
{
}

void WindowApp::OnResuming(Platform::Object^ sender, Platform::Object^ args)
{
}

void WindowApp::OnWindowSizeChanged(CoreWindow^ sender, WindowSizeChangedEventArgs^ args)
{
}

void WindowApp::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
	m_windowVisible = args->Visible;
}

void WindowApp::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
{
	m_windowClosed = true;
}

void WindowApp::OnDpiChanged(DisplayInformation^ sender, Object^ args)
{
}

void WindowApp::OnOrientationChanged(DisplayInformation^ sender, Object^ args)
{
}

void WindowApp::OnDisplayContentsInvalidated(DisplayInformation^ sender, Object^ args)
{
}