#include <stdafx.h>
#include <engine/dynlib/dynlib.h>

#include GY_PLATINC_DYNLIB

uint32_t gyDynLib::ComputeHash()
{
  return PreComputeHash(libFilename.c_str());
}

uint32_t gyDynLib::PreComputeHash(const char* libFilename)
{
  return gyComputeHash( libFilename, (int32_t)gyStringUtil::StrLen(libFilename) );
}

gyDynLibManager::gyDynLibManager()
{
  impl = new Impl;
}

gyDynLibManager::~gyDynLibManager()
{
  CloseAll();
  SafeDelete(impl);
}

void gyDynLibManager::CloseAll()
{
  dynlibs.ReleaseAll();
}

gyIDDynLib gyDynLibManager::LibOpen( const char* dynamicLib )
{
  if ( !dynamicLib )
    return gyIDDynLib::INVALID();
  
  // does exist? return it
  gyIDDynLib existId = dynlibs.FindByHash( gyDynLib::PreComputeHash(dynamicLib) );
  if ( existId.IsValid() )
    return existId;

  // not exists..create and add it
  gySharedPtr<gyDynLib> dynLib = impl->DynLibOpen(dynamicLib);
  if ( !dynLib )
    return gyIDDynLib::INVALID();

  return dynlibs.AddUnique(dynLib);
}

void gyDynLibManager::LibClose( gyIDDynLib& idDynLib )
{
  if ( !idDynLib.IsValid() )
    return;
  dynlibs.Release(idDynLib);
}

bool gyDynLibManager::LibReopen( gyIDDynLib idDynLib )
{
  IMPLEMENT("gyDynLibManager::DynLibReopen");
  return false;
}

// gyVariant gyDynLibManager::CallFunction( gyIDDynLib idLib, const char* funcName, gyVariant param)
// {
//   gyDynLib* lib = dynlibs.Get(idLib);
//   if ( !lib )
//     return 0;
// 
//   return 0;
// }
// 
bool gyDynLibManager::LibGetFunctions( gyIDDynLib idDynLib, void* functions, const char** funcNames )
{
  gyDynLib* lib = dynlibs.Get(idDynLib);
  if ( !lib ) return false;
  return impl->DynLibGetFunctions(lib, functions, funcNames);
} 


