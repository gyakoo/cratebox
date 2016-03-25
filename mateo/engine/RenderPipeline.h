#pragma once
#include <engine/DxTypes.h>

namespace Engine
{
  class StepTimer;
  
  class RenderPipeline : public DxLifetime
  {
  public:
    RenderPipeline();
    virtual ~RenderPipeline();

    virtual void Update(const StepTimer& timer);
    virtual void Render();

    virtual void CreateResources() override;
    virtual void ReleaseResources() override;
    virtual void ReloadWindowSizeResources() override;
  };

};