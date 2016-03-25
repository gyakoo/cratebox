#ifndef _GY_DYNLIBMANAGER_H_
#define _GY_DYNLIBMANAGER_H_

#define GY_DYNLIB_FUNCTION(ret, nam, proto) typedef ret (__cdecl* T##nam)proto; T##nam nam

//////////////////////////////////////////////////////////////////////////
class gyEngine;
struct gyDynLib : public gyResourceBase
{
  std::string libFilename;

  gy_override uint32_t ComputeHash();

  static uint32_t PreComputeHash(const char* libFilename);
};

//////////////////////////////////////////////////////////////////////////
class gyDynLibManager : public gyReflectedObject
{
  GY_PIMPL_CLASS();
  GY_DECLARE_REFLECT(gyDynLibManager);
public:
  void CloseAll();

  gyIDDynLib LibOpen( const char* dynamicLib );
  void LibClose( gyIDDynLib& idDynLib );
  bool LibReopen( gyIDDynLib idDynLib );
  bool LibGetFunctions( gyIDDynLib idDynLib, void* functions, const char** funcNames );

private:
  friend class gyEngine;
  gyDynLibManager();
  ~gyDynLibManager();
  gyResourceMgr<gyDynLib, ID_DYNLIB>  dynlibs;
  
};

#endif