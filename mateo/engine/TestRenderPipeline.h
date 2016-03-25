#pragma once

#include <engine/RenderPipeline.h>
#include <engine/Terrain.h>

namespace Engine
{
	// Renders the current FPS value in the bottom right corner of the screen using Direct2D and DirectWrite.
	class TestRenderPipeline : public RenderPipeline
	{
	public:
        TestRenderPipeline();		

        virtual void Update(const StepTimer& timer);
        virtual void Render();

        virtual void CreateResources() override;
        virtual void ReleaseResources() override;
        virtual void ReloadWindowSizeResources() override;

	private:
        std::unique_ptr<DirectX::EffectFactory> m_fxFactory;
        std::unique_ptr<DirectX::Model> m_model;

        Terrain     m_terrain;
        DirectX::SimpleMath::Matrix m_world, m_view, m_proj;
	};
}