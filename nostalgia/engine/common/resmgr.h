#ifndef _GY_RESOURCE_MANAGER_H_
#define _GY_RESOURCE_MANAGER_H_


/*
 * Tiene que soportar:
 *  - Resources que tienen el mismo hash, así pues no se introducen duplicados y devuelve el actual
 *  - Resources que aunque tengan el mismo hash, se permitan duplicados
 *  - Huecos en el array que pueden ser utilizados tras un tiempo
 *  - Cuando un resource es eliminado, su entrada se marca como eliminado, pero el slot sigue vivo durante
 *        un tiempo y podrá ser usado "después".
 */

class gyResourceBase : public gyRefCounted
{
public:
  virtual uint32_t ComputeHash(){ return 0; }
};

template<typename RES, int RESTYPE>
class gyResourceMgr
{
public:
  struct ResourceInfo
  {
    ResourceInfo() : hash(0){}
    void Destroy();
    bool IsAvailable() const;

    gySharedPtr<RES>  resource;
    uint32_t            hash;
  };

public:
  typedef gyResourceID<RESTYPE> IDResource;
  typedef RES ResourceType;
  ~gyResourceMgr();

  IDResource FindByHash(uint32_t hash);
  IDResource AddUnique(RES* res, bool* exists=0);
  template<typename _pred>
  IDResource AddUnique(RES* res, _pred pred, bool* exists=0);
  IDResource Add(RES* res);
  ResourceType* Get(IDResource idres);

  bool Release(IDResource& idres);
  void ReleaseAll(bool deallocate=true);

  int GetSize(){ return (int)resources.size(); }
  ResourceType* GetByNdx(int ndx){ return (ndx>=0 && ndx<GetSize()) ? resources[ndx].resource : NULL; }
  int GetValidElementCount()const;

private:
  typedef std::vector<ResourceInfo> ResourceList;
  ResourceList resources;
};

#include <engine/common/resmgr.inl>
#endif