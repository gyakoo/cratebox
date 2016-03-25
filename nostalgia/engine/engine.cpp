#include <stdafx.h>
#include <engine/engine.h>
#include <engine/file/file.h>
#include <engine/gfx/renderer.h>
#include <engine/common/timer.h>
#include <engine/job/jobmgr.h>
#include <engine/input/input.h>
#include <engine/reflect/reflect.h>
#include <engine/sound/soundmgr.h>
#include <engine/dynlib/dynlib.h>
#include <engine/net/netmgr.h>
#include <engine/physics/physics2d.h>
#include <engine/app/app.h>
#include <engine/log/logmgr.h>

enum { BIGSTACKCHUNKSIZE = 8*1024*1024, MEDIUMSTACKCHUNKSIZE = 1*1024*1024 }; // in bytes (8 and 1 MBs)

// forwards
// ------------------------------------------------------------
unsigned int MurmurHash2 ( const void * key, int len, unsigned int seed );

  // ------------------------------------------------------------
#ifndef BX_PLATFORM_WINDOWS
#include <assert.h>
#undef ASSERT
#define ASSERT(c,m) assert(c && m)
#endif

#include GY_PLATINC_ATOMIC
#include GY_PLATINC_ENGINE
//-----------------------------------------------------------------------------
uint32_t gyComputeHash( const void* data, int32_t len, uint32_t seed/*=HASH_SEED*/ )
{
  return (uint32_t)MurmurHash2( data, (int)len, (unsigned int)seed );
}

void gySleep(int ms)
{
#ifdef BX_PLATFORM_WINDOWS
  ::Sleep((DWORD)ms);
#else
  IMPLEMENT("No sleep defined for this platform");
#endif
}

//-----------------------------------------------------------------------------
// MurmurHash2, by Austin Appleby
// Note - This code makes a few assumptions about how your machine behaves -
// 1. We can read a 4-byte value from any address without crashing
// 2. sizeof(int) == 4
// And it has a few limitations -
// 1. It will not work incrementally.
// 2. It will not produce the same results on little-endian and big-endian
//    machines.
unsigned int MurmurHash2 ( const void * key, int len, unsigned int seed )
{
  // 'm' and 'r' are mixing constants generated offline.
  // They're not really 'magic', they just happen to work well.

  const unsigned int m = 0x5bd1e995;
  const int r = 24;

  // Initialize the hash to a 'random' value

  unsigned int h = seed ^ len;

  // Mix 4 bytes at a time into the hash

  const unsigned char * data = (const unsigned char *)key;

  while(len >= 4)
  {
    unsigned int k = *(unsigned int *)data;

    k *= m; 
    k ^= k >> r; 
    k *= m; 

    h *= m; 
    h ^= k;

    data += 4;
    len -= 4;
  }

  // Handle the last few bytes of the input array

  switch(len)
  {
  case 3: h ^= data[2] << 16;
  case 2: h ^= data[1] << 8;
  case 1: h ^= data[0];
    h *= m;
  };

  // Do a few final mixes of the hash to ensure the last few
  // bytes are well-incorporated.

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  return h;
} 

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef BX_PLATFORM_WINDOWS
void gyPerfMarkerBegin( const char* name ){ }
void gyPerfMarkerEnd(){ }
#endif    

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if !defined(_DEBUG) && defined(PROFILE)
void gyProfileGPUBegin(const char* name ){}
void gyProfileGPUEnd(){}
#else
void gyProfileGPUBegin( const char* name )
{
  GYUNUSED(name);
}
void gyProfileGPUEnd()
{

}
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef BX_PLATFORM_WINDOWS
int gyGetCommandLine(char**& argv){ return 0; }
void gyFreeCommandLine(){}
#endif

//-----------------------------------------------------------------------------
//-------------------------------------------------------------------------
uint8_t* gyGetBigStackChunkST()
{
  static char s_bigchunk[BIGSTACKCHUNKSIZE];
  return (uint8_t*)s_bigchunk;
}
uint64_t gyGetBigStackChunkSTSize()
{
  return BIGSTACKCHUNKSIZE;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
gyEngine& gyEngine::GetInstance()
{
  static gyEngine s_instance;
  return s_instance;
}

gyEngine::gyEngine()
  : application(NULL), renderer(NULL), fileSystem(NULL), jobManager(NULL)
  , inputSystem(NULL), timer(NULL), commonDict(NULL), soundManager(NULL)
  , dynLibManager(NULL), netManager(NULL), logManager(NULL)
{
}

gyEngine::~gyEngine()
{
  Release();
}

int gyEngine::Create(const gyVarDict& params)
{
  // allocate objects
  if ( !logManager && params.Has("gyLogManager") )  { logManager = new gyLogManager; logManager->AddRef(); }
  if ( !commonDict )  commonDict = new gyVarDict; 
  if ( !fileSystem )  { fileSystem = new gyFileSystem; fileSystem->AddRef(); }
  if ( !renderer )    { renderer = new gyRenderer; renderer->AddRef(); }
  if ( !inputSystem ) { inputSystem = new gyInput; inputSystem->AddRef(); }
  if ( !jobManager && params.Has("gyJobManager") )  { jobManager = new gyJobManager; jobManager->AddRef(); }
  const char* appclassname = params["appclass"].GetS();
  if ( !application && appclassname ) { application = (gyApp*)gyGetReflect().CreateInstance(appclassname); if ( application ) application->AddRef(); }
  if ( !application ) { application = new gyDefaultApp; application->AddRef(); }
  if ( !timer )       { timer = new gyTimer; timer->AddRef(); }
  if ( !reflect )     { reflect = new gyReflect; reflect->AddRef(); }
  if ( !soundManager && params.Has("gySoundManager") ){ soundManager = new gySoundManager; soundManager->AddRef(); }  
  if ( !physicsManager2D && params.Has("gyPhysicsManager2D")){ physicsManager2D = new gyPhysicsManager2D; physicsManager2D->AddRef(); }
  
  // dynLibManager -> on demand
  // netManager -> on demand
  
  // initialize them in proper order
  gyVarDict finalParams = params;
  if ( PrepareCreateParams(finalParams) != R_OK )
    return -1;

  if ( CreateModules(finalParams) != R_OK )
    return -1;

  return R_OK;
}

void gyEngine::Release()
{
  SafeRelease(physicsManager2D);
  SafeRelease(netManager);
  SafeRelease(dynLibManager);
  SafeRelease(soundManager);
  SafeRelease(reflect);
  SafeRelease(timer);
  SafeRelease(jobManager);
  SafeRelease(inputSystem);
  SafeRelease(renderer);
  SafeRelease(fileSystem);
  SafeRelease(logManager);
  SafeRelease(application);  
  SafeDelete(commonDict);
}

gyRenderer& gyEngine::GetRenderer()
{
  FAIL_IF(!renderer, "Not initialized");
  return *renderer; 
}
gyFileSystem& gyEngine::GetFileSystem()
{ 
  if ( !fileSystem )
  {
    fileSystem = new gyFileSystem;
    fileSystem->AddRef();
  }
  return *fileSystem; 
}
gyJobManager& gyEngine::GetJobManager()
{ 
  if ( !jobManager )
  {
    jobManager = new gyJobManager;
    jobManager->AddRef(); 
  }
  return *jobManager; 
}
gyInput& gyEngine::GetInputSystem()
{ 
  FAIL_IF(!inputSystem, "Not initialized");
  return *inputSystem; 
}
gyApp& gyEngine::GetApplication()
{ 
  FAIL_IF(!application, "Not initialized");
  return *application; 
}
gyTimer& gyEngine::GetTimer()
{
  FAIL_IF(!timer, "Not initialized");
  return *timer;
}

gyReflect& gyEngine::GetReflection()
{
  if ( !reflect )
  {
    reflect = new gyReflect;
    reflect->AddRef();
  }
  return *reflect;
}

gyVarDict& gyEngine::GetCommonDict()
{
  if ( !commonDict )
    commonDict = new gyVarDict;
  return *commonDict;
}

gySoundManager& gyEngine::GetSoundManager()
{
  if ( !soundManager )
  {
    soundManager = new gySoundManager;
    soundManager->AddRef();
  }
  return *soundManager;
}

gyDynLibManager& gyEngine::GetDynLibManager()
{
  if ( !dynLibManager )
  {
    dynLibManager = new gyDynLibManager();
    dynLibManager->AddRef();
  }
  return *dynLibManager;
}

gyNetManager& gyEngine::GetNetManager()
{
  if ( !netManager )
  {
    netManager = new gyNetManager();
    netManager->AddRef();
  }
  return *netManager;
}

gyPhysicsManager2D& gyEngine::GetPhysicsManager2D()
{
  FAIL_IF(!physicsManager2D, "No physics manager 2d initialized" );  
  return *physicsManager2D;
}

gyLogManager& gyEngine::GetLogManager()
{
  if ( !logManager )
  {
    logManager = new gyLogManager();
    logManager->AddRef();
  }
  return *logManager;
}

int gyEngine::Run()
{
  return application->Run();
}

int gyEngine::PrepareCreateParams(gyVarDict& params)
{
  gyReflectedObject* neededSpaces[] = { logManager, fileSystem, renderer, inputSystem, jobManager, timer, soundManager, physicsManager2D };
  int count = sizeof(neededSpaces)/sizeof(gyReflectedObject*);
  for ( int i = 0; i<count != NULL; ++i )
  {
    if ( !neededSpaces[i] ) continue;
    const char* className = neededSpaces[i]->GetNameOfClass();
    if ( !params.Has(className) )
      params[className] = gyVarDict();
    neededSpaces[i]->FillCreateTemplate(params[className].GetVT());
  }

  // application uses base class name
  if ( !params.Has("gyApp") )
    params["gyApp"] = gyVarDict();
  application->FillCreateTemplate(params["gyApp"].GetVT());

  return R_OK;
}

int gyEngine::CreateModules(gyVarDict& params)
{
  gyReflectedObject* modules[] = { logManager, fileSystem, application, renderer, inputSystem, jobManager, timer, soundManager, physicsManager2D };
  int count = sizeof(modules)/sizeof(gyReflectedObject*);
  for ( int i = 0; i < count; ++i )
  {
    if ( !modules[i] ) continue;
    const char* className = ( modules[i] != application ) ? modules[i]->GetNameOfClass() : "gyApp";
    GYINFO("Initializing %s...", className);
    if ( modules[i]->Create(params[className].GetVT()) != R_OK )
      return -1;
  }

  return R_OK;
}
