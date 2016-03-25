#pragma once
#include <engine/DxTypes.h>

namespace Engine
{
    class StepTimer;

    class Terrain : public DxLifetime
    {
    public:
        void Update(const StepTimer& timer);
        void Render();

        virtual void CreateResources() override;
        virtual void ReleaseResources() override;
        virtual void ReloadWindowSizeResources() override;
    private:
    };


}