#pragma once

#include <engine/RenderPipeline.h>

namespace Engine
{
	// Renders the current FPS value in the bottom right corner of the screen using Direct2D and DirectWrite.
	class TestRenderPipeline : public RenderPipeline
	{
	public:
        TestRenderPipeline();		

        virtual void Update(const StepTimer& timer);
        virtual void Render();

        virtual void createResources();
        virtual void releaseResources();
        virtual void ReloadWindowSizeResources();

	private:
		// Resources related to text rendering.
		std::wstring                                    m_text;
		DWRITE_TEXT_METRICS	                            m_textMetrics;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    m_whiteBrush;
		Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock1> m_stateBlock;
		Microsoft::WRL::ComPtr<IDWriteTextLayout3>      m_textLayout;
		Microsoft::WRL::ComPtr<IDWriteTextFormat2>      m_textFormat;

        IdTexture   m_mytex;
        IdByteCode  m_psByteCode;
        IdShader    m_ps;
        IdConstantBuffer m_psCb;
	};
}