#include <Pch.h>
#include <engine/TestRenderPipeline.h>
#include <engine/DxHelper.h>
#include <engine/DxDevice.h>
#include <engine/StepTimer.h>

using namespace Engine;
using namespace Microsoft::WRL;
using namespace DirectX;
using namespace SimpleMath;

// Initializes D2D resources used for text rendering.
TestRenderPipeline::TestRenderPipeline()
{
    m_proj = Matrix::CreatePerspectiveFieldOfView(45.0f, 1.0f, 0.01f, 100.0f);
    m_world = Matrix::Identity;
    m_view = Matrix::CreateLookAt(Vector3(0,0,10), Vector3::Zero, Vector3(0, 1, 0));
}

// Updates the text to be displayed.
void TestRenderPipeline::Update(const StepTimer& timer)
{
    auto DxDevice = DxDevice::GetInstance();
    m_terrain.Update(timer);
}

// Renders a frame to the screen.
void TestRenderPipeline::Render()
{
    auto dxDev = DxDevice::GetInstance();
    auto& context = dxDev->GetDefaultContext();
    auto& factory = dxDev->GetFactory();

    context.SetViewport(dxDev->GetScreenViewport());
    context.SetRenderTargetDefault();
    context.CmdClearRenderTargetDefault(DirectX::Colors::CornflowerBlue);
    context.Apply();

    m_terrain.Render();
    if (m_model)
    {
        m_model->Draw(dxDev->GetD3DDeviceContext(), factory.getCommonStates(), m_world, m_view, m_proj);
    }
}

void TestRenderPipeline::CreateResources()
{
    RenderPipeline::CreateResources();

    auto dxDevice = DxDevice::GetInstance()->GetD3DDevice();
    m_fxFactory = std::make_unique<DirectX::EffectFactory>(dxDevice);
    
    ReadDataAsync(L"engine\\scene.cmo").then([dxDevice,this](const std::vector<byte>& filedata)
    {
        m_model = DirectX::Model::CreateFromCMO(dxDevice, (uint8_t*)&filedata[0], filedata.size(), *m_fxFactory);
    });

    m_terrain.CreateResources();
}
void TestRenderPipeline::ReleaseResources()
{
    RenderPipeline::ReleaseResources();

    m_terrain.ReleaseResources();
    m_fxFactory.reset();
    m_model.reset();
}

void TestRenderPipeline::ReloadWindowSizeResources()
{
    RenderPipeline::ReloadWindowSizeResources();

}