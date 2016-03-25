
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
struct gyDynLibDLL : public gyDynLib
{ 
  gyDynLibDLL();
  ~gyDynLibDLL();
  HMODULE dllHandle;
};

gyDynLibDLL::gyDynLibDLL()
  : dllHandle(NULL)
{
}

gyDynLibDLL::~gyDynLibDLL()
{
  if ( dllHandle != NULL )
    FreeLibrary(dllHandle);
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class gyDynLibManager::Impl
{
public:
  gyDynLib* DynLibOpen( const char* dynamicLib );
  bool DynLibGetFunctions(gyDynLib* lib, void* functions, const char** funcNames);
};

gyDynLib* gyDynLibManager::Impl::DynLibOpen( const char* dynamicLib )
{
  HMODULE dllHandle = LoadLibraryA(dynamicLib);
  if ( !dllHandle )
    return NULL;

  gyDynLibDLL* dllLib = new gyDynLibDLL;
  dllLib->dllHandle = dllHandle;
  dllLib->libFilename = dynamicLib;

  typedef int (__cdecl *T_filterNumber)(float);
  T_filterNumber filterNumber = (T_filterNumber)GetProcAddress(dllLib->dllHandle, "filterNumber");
  return dllLib;
}

bool gyDynLibManager::Impl::DynLibGetFunctions(gyDynLib* lib, void* functions, const char** funcNames)
{
  gyDynLibDLL* dll = (gyDynLibDLL*)lib;
  if ( !dll || !dll->dllHandle )
    return false;

  ptrType* funcPtrs = (ptrType*)functions;
  for ( int i = 0; funcNames[i]; ++i )
  {
    FARPROC f = GetProcAddress(dll->dllHandle, funcNames[i]);
    *funcPtrs = (ptrType)f;
    funcPtrs++;
  }
  return true;
}

