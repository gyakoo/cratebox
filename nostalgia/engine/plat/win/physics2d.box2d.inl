#include <thirdparty/box2d/inc/Box2D.h>
// #ifdef _DEBUG
// #pragma comment(lib,"Box2Dd.lib")
// #else
// #pragma comment(lib,"Box2D.lib")
// #endif

/////////////////////////////////////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////////////////////////////////////
class gyPhysicsManager2D::Impl
{
public:
  Impl();
  ~Impl(){ Destroy(); }
  int Create(gyVarDict& createParams);
  void Destroy();
private: 
  b2World* world;
};

int gyPhysicsManager2D::Impl::Create(gyVarDict& createParams)
{
  b2Vec2 gravity;
  gravity.x = createParams.Get("gravX",0.0f);
  gravity.y = createParams.Get("gravY",-10.0f);
  world = new b2World(gravity);
  return R_OK;
}

gyPhysicsManager2D::Impl::Impl() : world(NULL)
{ 
}

void gyPhysicsManager2D::Impl::Destroy()
{
  SafeDelete(world);
}

