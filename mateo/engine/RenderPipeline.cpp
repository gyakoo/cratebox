#include <Pch.h>
#include <engine/RenderPipeline.h>
#include <engine/StepTimer.h>

namespace Engine
{
  RenderPipeline::RenderPipeline()
  {
  }

  RenderPipeline::~RenderPipeline()
  {
      ReleaseResources();
  }

  void RenderPipeline::Update(const StepTimer& timer)
  {

  }

  void RenderPipeline::Render()
  {

  }

  void RenderPipeline::CreateResources()
  {

  }

  void RenderPipeline::ReleaseResources()
  {

  }

  void RenderPipeline::ReloadWindowSizeResources()
  {

  }

};