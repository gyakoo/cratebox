#include <stdafx.h>
#include <engine/physics/physics2d.h>

#include GY_PLATINC_PHYSICS2D

int gyPhysicsManager2D::Create(gyVarDict& createParams)
{
  return impl->Create(createParams);
}

void gyPhysicsManager2D::FillCreateTemplate(gyVarDict& outTemplate)
{
  
}

gyPhysicsManager2D::gyPhysicsManager2D()
{
  impl = new Impl();
}

gyPhysicsManager2D::~gyPhysicsManager2D()
{
  Destroy();
  SafeDelete(impl);
}

void gyPhysicsManager2D::Destroy()
{ 
  impl->Destroy();
}
