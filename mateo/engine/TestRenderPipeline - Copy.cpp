#include <Pch.h>
#include <engine/TestRenderPipeline.h>
#include <engine/DxHelper.h>
#include <engine/DxDevice.h>
#include <engine/StepTimer.h>

using namespace Engine;
using namespace Microsoft::WRL;

// Initializes D2D resources used for text rendering.
TestRenderPipeline::TestRenderPipeline()
{
	ZeroMemory(&m_textMetrics, sizeof(DWRITE_TEXT_METRICS));

	// Create device independent resources
	ComPtr<IDWriteTextFormat> textFormat;
	Engine::ThrowIfFailed(
		DxDevice::GetInstance()->GetDWriteFactory()->CreateTextFormat(
			L"Segoe UI",
			nullptr,
			DWRITE_FONT_WEIGHT_LIGHT,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			32.0f,
			L"en-US",
			&textFormat
			)
		);

    Engine::ThrowIfFailed(
		textFormat.As(&m_textFormat)
		);

    Engine::ThrowIfFailed(
		m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR)
		);

    Engine::ThrowIfFailed(
		DxDevice::GetInstance()->GetD2DFactory()->CreateDrawingStateBlock(&m_stateBlock)
		);
}

// Updates the text to be displayed.
void TestRenderPipeline::Update(const StepTimer& timer)
{
    auto DxDevice = DxDevice::GetInstance();

    // Update display text.
	uint32 fps = timer.GetFramesPerSecond();

	m_text = (fps > 0) ? std::to_wstring(fps) + L" FPS" : L" - FPS";

	ComPtr<IDWriteTextLayout> textLayout;
    Engine::ThrowIfFailed(
        DxDevice->GetDWriteFactory()->CreateTextLayout(
			m_text.c_str(),
			(uint32) m_text.length(),
			m_textFormat.Get(),
			240.0f, // Max width of the input text.
			50.0f, // Max height of the input text.
			&textLayout
			)
		);

    Engine::ThrowIfFailed(
		textLayout.As(&m_textLayout)
		);

    Engine::ThrowIfFailed(
		m_textLayout->GetMetrics(&m_textMetrics)
		);
}

// Renders a frame to the screen.
void TestRenderPipeline::Render()
{
    auto DxDevice = DxDevice::GetInstance();

    // clear render target
    {
        auto context = DxDevice->GetD3DDeviceContext();

        // Reset the viewport to target the whole screen.
        auto viewport = DxDevice->GetScreenViewport();
        context->RSSetViewports(1, &viewport);

        // Reset render targets to the screen.
        ID3D11RenderTargetView *const targets[1] = { DxDevice->GetBackBufferRenderTargetView() };
        context->OMSetRenderTargets(1, targets, DxDevice->GetDepthStencilView());

        // Clear the back buffer and depth stencil view.
        context->ClearRenderTargetView(DxDevice->GetBackBufferRenderTargetView(), DirectX::Colors::CornflowerBlue);
        context->ClearDepthStencilView(DxDevice->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    }

	ID2D1DeviceContext* context = DxDevice::GetInstance()->GetD2DDeviceContext();
	Windows::Foundation::Size logicalSize = DxDevice::GetInstance()->GetLogicalSize();

	context->SaveDrawingState(m_stateBlock.Get());
	context->BeginDraw();

	// Position on the bottom right corner
	D2D1::Matrix3x2F screenTranslation = D2D1::Matrix3x2F::Translation(
		logicalSize.Width - m_textMetrics.layoutWidth,
		logicalSize.Height - m_textMetrics.height
		);

	context->SetTransform(screenTranslation * DxDevice::GetInstance()->GetOrientationTransform2D());

    Engine::ThrowIfFailed(
		m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING)
		);

	context->DrawTextLayout(
		D2D1::Point2F(0.f, 0.f),
		m_textLayout.Get(),
		m_whiteBrush.Get()
		);

	// Ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
	// is lost. It will be handled during the next call to Present.
	HRESULT hr = context->EndDraw();
	if (hr != D2DERR_RECREATE_TARGET)
	{
        Engine::ThrowIfFailed(hr);
	}

	context->RestoreDrawingState(m_stateBlock.Get());
}

void TestRenderPipeline::createResources()
{
    Engine::ThrowIfFailed(
        DxDevice::GetInstance()->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_whiteBrush)
		);

    auto& factory = DxDevice::GetInstance()->GetFactory();
    factory.createTexture(L"Content\\stones.jpg")
        .then([this](IdTexture texId) 
    {
        m_mytex = texId; 
    });

    factory.createShaderByteCode(L"SamplePixelShader.cso")
        .then([&factory, this](IdByteCode bcId)
    {
        m_ps = factory.createShader(bcId, SHADER_PIXEL);
        m_psCb = factory.createConstantBuffer(bcId, SHADER_PIXEL);
    } );
}
void TestRenderPipeline::releaseResources()
{
	m_whiteBrush.Reset();

    auto factory = DxDevice::GetInstance()->GetFactory();
    factory.releaseResource(m_ps);
    factory.releaseResource(m_psCb);
    factory.releaseResource(m_psByteCode);
    factory.releaseResource(m_mytex);
}

void TestRenderPipeline::ReloadWindowSizeResources()
{

}