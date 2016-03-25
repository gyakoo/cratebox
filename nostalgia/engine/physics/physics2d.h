#ifndef _GY_PHYSICS_MANAGER_2D_H_
#define _GY_PHYSICS_MANAGER_2D_H_

class gyEngine;

//////////////////////////////////////////////////////////////////////////
class gyPhysicsManager2D : public gyReflectedObject
{
  GY_PIMPL_CLASS();
  GY_DECLARE_REFLECT(gyPhysicsManager2D);
public:
  gy_override int Create(gyVarDict& createParams);
  gy_override void FillCreateTemplate(gyVarDict& outTemplate);

  // -- Manager control
  void Destroy();
  
private:
  friend class gyEngine;
  gyPhysicsManager2D();
  ~gyPhysicsManager2D();

private:
};

#endif