#pragma once

// Renders Direct2D and 3D content on the screen.
namespace Game
{
	class GameMain
	{
	public:
		GameMain(Windows::ApplicationModel::Core::CoreApplicationView^ appView);
		~GameMain();

        void Load(Platform::String^ entryPoint);
        void SetWindow(Windows::UI::Core::CoreWindow^ window);
        bool Run();

	private:
        Platform::Agile<Engine::Base> m_engineBase;		
	};
}